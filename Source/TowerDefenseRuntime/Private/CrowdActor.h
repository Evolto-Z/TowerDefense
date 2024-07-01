// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdActorInterface.h"
#include "CrowdAnimInstanceData.h"
#include "MassActorPoolableInterface.h"
#include "GameFramework/Character.h"
#include "CrowdActor.generated.h"

class UMassAgentComponent;

UCLASS()
class TOWERDEFENSERUNTIME_API ACrowdActor : public ACharacter, public IMassActorPoolableInterface, public ICrowdActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACrowdActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMassAgentComponent> AgentComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~Begin ICrowdActorInterface
	virtual void OnGetOrSpawn(FMassEntityManager* EntityManager, const FMassEntityHandle MassAgent) override;
	virtual void SetAdditionalMeshOffset(const float Offset) override;
	virtual FCrowdAnimInstanceData& GetMutableAnimInstanceData() override;
	//~End ICrowdActorInterface

	UPROPERTY(Transient)
	FCrowdAnimInstanceData AnimInstanceData;
};
