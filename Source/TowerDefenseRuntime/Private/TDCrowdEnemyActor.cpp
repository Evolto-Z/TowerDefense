// Fill out your copyright notice in the Description page of Project Settings.


#include "TDCrowdEnemyActor.h"

#include "MassAgentComponent.h"


// Sets default values
ATDCrowdEnemyActor::ATDCrowdEnemyActor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgentComponent = CreateDefaultSubobject<UMassAgentComponent>(TEXT("MassAgent"));
}

// Called when the game starts or when spawned
void ATDCrowdEnemyActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATDCrowdEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATDCrowdEnemyActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

