// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassCrowdRepresentationActorManagement.h"
#include "CrowdRepresentationManagement.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="Tower Denfense Crowd Visualization"))
class TOWERDEFENSERUNTIME_API UCrowdRepresentationManagement : public UMassCrowdRepresentationActorManagement
{
	GENERATED_BODY()

	virtual EMassActorSpawnRequestAction OnPostActorSpawn(const FMassActorSpawnRequestHandle& SpawnRequestHandle, FConstStructView SpawnRequest, TSharedRef<FMassEntityManager> EntityManager) const override;

	virtual void TeleportActor(const FTransform& Transform, AActor& Actor, FMassCommandBuffer& CommandBuffer) const override;
};
