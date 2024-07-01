// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdAnimInstanceData.h"
#include "MassEntityTypes.h"
#include "UObject/Interface.h"
#include "CrowdActorInterface.generated.h"

struct FMassEntityManager;
// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UCrowdActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TOWERDEFENSERUNTIME_API ICrowdActorInterface
{
	GENERATED_BODY()

public:
	virtual void OnGetOrSpawn(FMassEntityManager* EntityManager, const FMassEntityHandle MassAgent) = 0;
	virtual void SetAdditionalMeshOffset(const float Offset) = 0;
	virtual FCrowdAnimInstanceData& GetMutableAnimInstanceData() = 0;
};
