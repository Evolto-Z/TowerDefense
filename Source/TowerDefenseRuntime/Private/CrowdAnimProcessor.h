// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassObserverProcessor.h"
#include "MassProcessor.h"
#include "CrowdAnimProcessor.generated.h"

/**
 * 
 */
UCLASS()
class UCrowdAnimInitializer : public UMassObserverProcessor
{
	GENERATED_BODY()

public:
	UCrowdAnimInitializer();

protected:
	virtual void ConfigureQueries() override;
	virtual void Initialize(UObject& Owner) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	UPROPERTY()
	UWorld* World = nullptr;

	FMassEntityQuery EntityQuery;
};

/**
 * 
 */
UCLASS()
class UCrowdAnimProcessor : public UMassProcessor
{
	GENERATED_BODY()

public:
	UCrowdAnimProcessor();

	UPROPERTY(EditAnywhere, Category="Animation", meta=(ClampMin=0.0, UIMin=0.0))
	float MoveThresholdSq = 750.0f;

private:
	void UpdateAnimationFragmentData(FMassEntityManager& EntityManager, FMassExecutionContext& Context, float GlobalTime, TArray<FMassEntityHandle, TInlineAllocator<32>>& ActorEntities);
	void UpdateVertexAnimationState(FMassEntityManager& EntityManager, FMassExecutionContext& Context, float GlobalTime);
	void UpdateSkeletalAnimation(FMassEntityManager& EntityManager, float GlobalTime, TArrayView<FMassEntityHandle> ActorEntities);

protected:
	/** Configure the owned FMassEntityQuery instances to express processor's requirements */
	virtual void ConfigureQueries() override;
	virtual void Initialize(UObject& Owner) override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	static UAnimInstance* GetAnimInstanceFromActor(const class AActor* Actor);

	UPROPERTY(Transient)
	UWorld* World = nullptr;

	FMassEntityQuery EntityQuery;
};
