// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdActor.h"

#include "AnimToTextureDataAsset.h"
#include "CrowdAnimTrait.h"
#include "MassAgentComponent.h"
#include "MassCommonFragments.h"
#include "MassLookAtFragments.h"
#include "MassMovementFragments.h"
#include "MassRepresentationFragments.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ACrowdActor::ACrowdActor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgentComponent = CreateDefaultSubobject<UMassAgentComponent>(TEXT("MassAgent"));
}

// Called when the game starts or when spawned
void ACrowdActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACrowdActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACrowdActor::OnGetOrSpawn(FMassEntityManager* EntityManager, const FMassEntityHandle MassAgent)
{
	if (EntityManager && EntityManager->IsEntityActive(MassAgent))
	{
		if (FMassVelocityFragment* MassVelocityFragment = EntityManager->GetFragmentDataPtr<FMassVelocityFragment>(MassAgent))
		{
			if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
			{
				CharMovement->Velocity = MassVelocityFragment->Value;
			}
		}

		// Copy mass data relevant to animation to a persistent place, to have a correct first frame of anim
		if (FTransformFragment* TransformFragment = EntityManager->GetFragmentDataPtr<FTransformFragment>(MassAgent))
		{
			AnimInstanceData.MassEntityTransform = TransformFragment->GetTransform();
		}

		if (FMassLookAtFragment* LookAtFragment = EntityManager->GetFragmentDataPtr<FMassLookAtFragment>(MassAgent))
		{
			AnimInstanceData.LookAtDirection = LookAtFragment->Direction;
		}

		AnimInstanceData.FarLODAnimSequence = nullptr;
		AnimInstanceData.FarLODPlaybackStartTime = 0.0f;
		if (FCrowdAnimFragment* AnimFragment = EntityManager->GetFragmentDataPtr<FCrowdAnimFragment>(MassAgent))
		{
			if (AnimFragment->AnimToTextureData.IsValid() && AnimFragment->AnimToTextureData->Animations.IsValidIndex(AnimFragment->AnimationStateIndex))
			{
				AnimInstanceData.FarLODAnimSequence = AnimFragment->AnimToTextureData->AnimSequences[AnimFragment->AnimationStateIndex].AnimSequence;
				if (AnimInstanceData.FarLODAnimSequence)
				{
					AnimInstanceData.FarLODAnimSequence = AnimFragment->AnimToTextureData->AnimSequences[AnimFragment->AnimationStateIndex].AnimSequence;

					if (UWorld* World = GetWorld())
					{
						const float GlobalTime = World->GetTimeSeconds();
						const float SequenceLength = AnimInstanceData.FarLODAnimSequence->GetPlayLength();
						AnimInstanceData.FarLODPlaybackStartTime = FMath::Fmod(AnimFragment->GlobalStartTime - GlobalTime, SequenceLength);

						if (AnimInstanceData.FarLODPlaybackStartTime < 0.0f)
						{
							AnimInstanceData.FarLODPlaybackStartTime += SequenceLength;
						}
					}
				}
			}
			AnimInstanceData.bSwappedThisFrame = AnimFragment->bSwappedThisFrame;
		}

		if (FMassRepresentationLODFragment* RepresentationFragment = EntityManager->GetFragmentDataPtr<FMassRepresentationLODFragment>(MassAgent))
		{
			AnimInstanceData.Significance = RepresentationFragment->LODSignificance;
		}
	}
}

void ACrowdActor::SetAdditionalMeshOffset(const float Offset)
{
	
}

FCrowdAnimInstanceData& ACrowdActor::GetMutableAnimInstanceData()
{
	return AnimInstanceData;
}
