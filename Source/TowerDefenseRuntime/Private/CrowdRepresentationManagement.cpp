// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdRepresentationManagement.h"

#include "CrowdActorInterface.h"
#include "MassActorSpawnerSubsystem.h"
#include "StructView.h"
#include "Components/CapsuleComponent.h"

EMassActorSpawnRequestAction UCrowdRepresentationManagement::OnPostActorSpawn(
	const FMassActorSpawnRequestHandle& SpawnRequestHandle, FConstStructView SpawnRequest,
	TSharedRef<FMassEntityManager> EntityManager) const
{
	const EMassActorSpawnRequestAction Result = Super::OnPostActorSpawn(SpawnRequestHandle, SpawnRequest, EntityManager);
	
	const FMassActorSpawnRequest& MassActorSpawnRequest = SpawnRequest.Get<const FMassActorSpawnRequest>();
	checkf(MassActorSpawnRequest.SpawnedActor, TEXT("Expecting valid spawned actor"));

	if (ICrowdActorInterface* CrowdActor = Cast<ICrowdActorInterface>(MassActorSpawnRequest.SpawnedActor))
	{
		CrowdActor->OnGetOrSpawn(&EntityManager.Get(), MassActorSpawnRequest.MassAgent);
	}

	return Result;
}

void UCrowdRepresentationManagement::TeleportActor(const FTransform& Transform, AActor& Actor,
	FMassCommandBuffer& CommandBuffer) const
{
	FTransform RootTransform = Transform;

	if (const UCapsuleComponent* CapsuleComp = Actor.FindComponentByClass<UCapsuleComponent>())
	{
		const FVector HalfHeight(0.0f, 0.0f, CapsuleComp->GetScaledCapsuleHalfHeight());
		RootTransform.AddToTranslation(HalfHeight);

		// TODO: Entity的Transform和Actor的Transform相差不大，应该不用做这个修正
		// const FVector RootLocation = RootTransform.GetLocation();
		// const FVector SweepOffset(0.0f, 0.0f, 20.0f);
		// const FVector Start = RootLocation + SweepOffset;
		// const FVector End = RootLocation - SweepOffset;
		// FCollisionQueryParams Params;
		// Params.AddIgnoredActor(&Actor);
		// FHitResult OutHit;
		// if (Actor.GetWorld()->SweepSingleByChannel(OutHit, Start, End, Transform.GetRotation(), CapsuleComp->GetCollisionObjectType(), CapsuleComp->GetCollisionShape(), Params))
		// {
		// 	if (ICrowdActorInterface* MassCrowdActor = Cast<ICrowdActorInterface>(&Actor))
		// 	{
		// 		MassCrowdActor->SetAdditionalMeshOffset(RootTransform.GetLocation().Z - OutHit.Location.Z);
		// 	}
		// 	RootTransform.SetLocation(OutHit.Location);
		// }
	}
	
	UMassRepresentationActorManagement::TeleportActor(RootTransform, Actor, CommandBuffer);
}
