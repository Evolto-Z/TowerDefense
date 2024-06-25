// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassActorPoolableInterface.h"
#include "GameFramework/Character.h"
#include "TDCrowdEnemyActor.generated.h"

class UMassAgentComponent;

UCLASS()
class TOWERDEFENSERUNTIME_API ATDCrowdEnemyActor : public ACharacter, public IMassActorPoolableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATDCrowdEnemyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMassAgentComponent> AgentComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
