// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAnimProcessor.h"

#include "AnimToTextureDataAsset.h"
#include "CrowdActorInterface.h"
#include "CrowdAnimTrait.h"
#include "MassActorSubsystem.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassEntityView.h"
#include "MassExecutionContext.h"
#include "MassLookAtFragments.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassRepresentationFragments.h"
#include "MassRepresentationTypes.h"
#include "GameFramework/Character.h"
#include "Steering/MassSteeringFragments.h"


UCrowdAnimInitializer::UCrowdAnimInitializer()
	: EntityQuery(*this)
{
	ObservedType = FCrowdAnimFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;

	ExecutionFlags = static_cast<int32>(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
}

void UCrowdAnimInitializer::ConfigureQueries()
{
	EntityQuery.AddRequirement<FCrowdAnimFragment>(EMassFragmentAccess::ReadWrite);
}

void UCrowdAnimInitializer::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	World = Owner.GetWorld();
}

void UCrowdAnimInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	check(World);
	const float GlobalTime = World->GetTimeSeconds();
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, GlobalTime](FMassExecutionContext& Context)
	{
		TArrayView<FCrowdAnimFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimFragment>();
		const int32 NumEntities = Context.GetNumEntities();
		for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
		{
			FCrowdAnimFragment& AnimationData = AnimationDataList[EntityIdx];
			// @todo: Replace this range w/ the length of the starting animation states.
			const float StartTimeOffset = FMath::FRandRange(0.0f, 10.0f);
			AnimationData.GlobalStartTime = GlobalTime - StartTimeOffset;
		}
	});
}


UCrowdAnimProcessor::UCrowdAnimProcessor()
	: EntityQuery(*this)
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Tasks;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::SyncWorldToMass);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Representation);

	bRequiresGameThreadExecution = true;
}

void UCrowdAnimProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassLookAtFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadOnly, EMassFragmentPresence::Optional);
	EntityQuery.AddRequirement<FCrowdAnimFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
	EntityQuery.RequireMutatingWorldAccess();
}

void UCrowdAnimProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	World = Owner.GetWorld();
	check(World);
}

void UCrowdAnimProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	check(World);

	QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_Run);

	const float GlobalTime = World->GetTimeSeconds();

	TArray<FMassEntityHandle, TInlineAllocator<32>> ActorEntities;
	{
		QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_UpdateAnimationFragmentData);
		EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, GlobalTime, &ActorEntities, &EntityManager](FMassExecutionContext& Context)
		{
			UpdateAnimationFragmentData(EntityManager, Context, GlobalTime, ActorEntities);
		});
	}
	{
		QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_UpdateVertexAnimationState);
		EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, GlobalTime, &EntityManager](FMassExecutionContext& Context)
		{
			UpdateVertexAnimationState(EntityManager, Context, GlobalTime);
		});
	}

	{
		QUICK_SCOPE_CYCLE_COUNTER(UMassProcessor_Animation_UpdateSkeletalAnimation);
		// Pull out UAnimToTextureDataAsset from the inner loop to avoid the resolve cost, which is extremely high in PIE.
		UpdateSkeletalAnimation(EntityManager, GlobalTime, MakeArrayView(ActorEntities));
	}
}

void UCrowdAnimProcessor::UpdateAnimationFragmentData(FMassEntityManager& EntityManager, FMassExecutionContext& Context, float GlobalTime, TArray<FMassEntityHandle, TInlineAllocator<32>>& ActorEntities)
{
	TArrayView<FCrowdAnimFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimFragment>();
	TConstArrayView<FMassRepresentationFragment> VisualizationList = Context.GetFragmentView<FMassRepresentationFragment>();
	TConstArrayView<FMassActorFragment> ActorInfoList = Context.GetFragmentView<FMassActorFragment>();

	const int32 NumEntities = Context.GetNumEntities();
	for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
	{
		FCrowdAnimFragment& AnimationData = AnimationDataList[EntityIdx];
		const FMassRepresentationFragment& Visualization = VisualizationList[EntityIdx];
		const FMassActorFragment& ActorFragment = ActorInfoList[EntityIdx];

		if (!ActorFragment.IsOwnedByMass())
		{
			continue;
		}

		const bool bWasActor = (Visualization.PrevRepresentation == EMassRepresentationType::HighResSpawnedActor) || (Visualization.PrevRepresentation == EMassRepresentationType::LowResSpawnedActor);
		const bool bIsActor = (Visualization.CurrentRepresentation == EMassRepresentationType::HighResSpawnedActor) || (Visualization.CurrentRepresentation == EMassRepresentationType::LowResSpawnedActor);
		AnimationData.bSwappedThisFrame = (bWasActor != bIsActor);

		switch (Visualization.CurrentRepresentation)
		{
		case EMassRepresentationType::LowResSpawnedActor:
		case EMassRepresentationType::HighResSpawnedActor:
		{
			FMassEntityHandle Entity = Context.GetEntity(EntityIdx);
			ActorEntities.Add(Entity);
			break;
		}
		default:
			break;
		}
	}
}

void UCrowdAnimProcessor::UpdateVertexAnimationState(FMassEntityManager& EntityManager, FMassExecutionContext& Context, float GlobalTime)
{
	const int32 NumEntities = Context.GetNumEntities();
	TArrayView<FCrowdAnimFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimFragment>();
	TConstArrayView<FMassRepresentationFragment> VisualizationList = Context.GetFragmentView<FMassRepresentationFragment>();
	TConstArrayView<FMassVelocityFragment> VelocityList = Context.GetFragmentView<FMassVelocityFragment>();

	for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
	{
		FCrowdAnimFragment& AnimationData = AnimationDataList[EntityIdx];

		const FMassRepresentationFragment& Visualization = VisualizationList[EntityIdx];
		const FMassVelocityFragment& Velocity = VelocityList[EntityIdx];

		// Need current anim state to update for skeletal meshes to do a smooth blend between poses
		if (Visualization.CurrentRepresentation != EMassRepresentationType::None)
		{
			int32 StateIndex = 0;
			{
				// @todo: Make a better way to map desired anim states here. Currently the anim texture index to access is hard-coded.
				const float VelocitySizeSq = Velocity.Value.SizeSquared();
				const bool bIsWalking = Velocity.Value.SizeSquared() > MoveThresholdSq;
				if(bIsWalking)
				{
					StateIndex = 1;
					const float AuthoredAnimSpeed = 140.0f;
					const float PrevPlayRate = AnimationData.PlayRate;
					AnimationData.PlayRate = FMath::Clamp(FMath::Sqrt(VelocitySizeSq / (AuthoredAnimSpeed * AuthoredAnimSpeed)), 0.8f, 2.0f);

					// Need to conserve current frame on a playrate switch so (GlobalTime - Offset1) * Playrate1 == (GlobalTime - Offset2) * Playrate2
					AnimationData.GlobalStartTime = GlobalTime - PrevPlayRate * (GlobalTime - AnimationData.GlobalStartTime) / AnimationData.PlayRate;
				}
				else
				{
					AnimationData.PlayRate = 1.0f;
					StateIndex = 0;
				}
			}
			AnimationData.AnimationStateIndex = StateIndex;
		}
	}
}

void UCrowdAnimProcessor::UpdateSkeletalAnimation(FMassEntityManager& EntityManager, float GlobalTime, TArrayView<FMassEntityHandle> ActorEntities)
{
	if (ActorEntities.Num() <= 0)
	{
		return;
	}

	for (FMassEntityHandle& Entity : ActorEntities)
	{
		FMassEntityView EntityView(EntityManager, Entity);

		FCrowdAnimFragment& AnimationData = EntityView.GetFragmentData<FCrowdAnimFragment>();
		FTransformFragment& TransformFragment = EntityView.GetFragmentData<FTransformFragment>();
		FMassActorFragment& ActorFragment = EntityView.GetFragmentData<FMassActorFragment>();
		
		const FMassLookAtFragment* LookAtFragment = EntityView.GetFragmentDataPtr<FMassLookAtFragment>();
		const FMassMoveTargetFragment* MovementTargetFragment = EntityView.GetFragmentDataPtr<FMassMoveTargetFragment>();
		const FMassSteeringFragment* SteeringFragment = EntityView.GetFragmentDataPtr<FMassSteeringFragment>();

		if (ICrowdActorInterface* CrowdActor = Cast<ICrowdActorInterface>(ActorFragment.GetMutable()))
		{
			FCrowdAnimInstanceData& AnimInstanceData = CrowdActor->GetMutableAnimInstanceData();
			
			AnimInstanceData.MassEntityTransform = TransformFragment.GetTransform();
			AnimInstanceData.LookAtDirection = LookAtFragment ? LookAtFragment->Direction : FVector::ForwardVector;
			
			AnimInstanceData.FarLODAnimSequence = nullptr;
			AnimInstanceData.FarLODPlaybackStartTime = 0.0f;
			if (AnimationData.AnimToTextureData.IsValid() && AnimationData.AnimToTextureData->Animations.IsValidIndex(AnimationData.AnimationStateIndex))
			{
				AnimInstanceData.FarLODAnimSequence = AnimationData.AnimToTextureData->AnimSequences[AnimationData.AnimationStateIndex].AnimSequence;
				if (AnimInstanceData.FarLODAnimSequence)
				{
					AnimInstanceData.FarLODAnimSequence = AnimationData.AnimToTextureData->AnimSequences[AnimationData.AnimationStateIndex].AnimSequence;

					const float SequenceLength = AnimInstanceData.FarLODAnimSequence->GetPlayLength();
					AnimInstanceData.FarLODPlaybackStartTime = FMath::Fmod(AnimationData.GlobalStartTime - GlobalTime,SequenceLength);

					if (AnimInstanceData.FarLODPlaybackStartTime < 0.0f)
					{
						AnimInstanceData.FarLODPlaybackStartTime += SequenceLength;
					}
				}
			}

			AnimInstanceData.Significance = EntityView.GetFragmentData<FMassRepresentationLODFragment>().LODSignificance;
			AnimInstanceData.bSwappedThisFrame = AnimationData.bSwappedThisFrame;

			if (SteeringFragment)
			{
				AnimInstanceData.DesiredVelocity = SteeringFragment->DesiredVelocity;
			}

			if (MovementTargetFragment)
			{
				AnimInstanceData.CurrentActionStartTime = MovementTargetFragment->GetCurrentActionStartTime();
				AnimInstanceData.CurrentActionID = MovementTargetFragment->GetCurrentActionID();
				AnimInstanceData.PreviousMovementAction = MovementTargetFragment->GetPreviousAction();
				AnimInstanceData.CurrentMovementAction = MovementTargetFragment->GetCurrentAction();

				AnimInstanceData.DistanceToEndOfPath = MovementTargetFragment->DistanceToGoal;
				AnimInstanceData.ActionAtEndOfPath = MovementTargetFragment->IntentAtGoal;
			}
		}
	}
}

UAnimInstance* UCrowdAnimProcessor::GetAnimInstanceFromActor(const AActor* Actor)
{
	const USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	if (const ACharacter* Character = Cast<ACharacter>(Actor))
	{
		SkeletalMeshComponent = Character->GetMesh();
	}
	else if (Actor)
	{
		SkeletalMeshComponent = Actor->FindComponentByClass<USkeletalMeshComponent>();
	}

	if (SkeletalMeshComponent)
	{
		return SkeletalMeshComponent->GetAnimInstance();
	}

	return nullptr;
}
