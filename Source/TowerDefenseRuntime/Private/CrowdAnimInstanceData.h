#pragma once
#include "MassNavigationTypes.h"

#include "CrowdAnimInstanceData.generated.h"

/**
 * 外部需要提供给CorAnimInstance的数据
 */
USTRUCT(BlueprintType)
struct FCrowdAnimInstanceData
{
	GENERATED_BODY()

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	UAnimSequence* FarLODAnimSequence = nullptr;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	FTransform MassEntityTransform;

	// In local/component space
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	FVector LookAtDirection = FVector::ForwardVector;
	
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	float FarLODPlaybackStartTime = 0.0f;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	float Significance = 0.0f;

	// Default to true to assume we always swapped on init
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	bool bSwappedThisFrame = true;

	// Current raw desired velocity from steering.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	FVector DesiredVelocity = FVector::ZeroVector;

	// Global time in seconds when the current action started.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	float CurrentActionStartTime = 0.0f;

	// Sequential action ID, updated as AI changes it's mind.
	uint16 CurrentActionID = 0;

	// Previous movement action.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	EMassMovementAction PreviousMovementAction = EMassMovementAction::Move;

	// Current movement action.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	EMassMovementAction CurrentMovementAction = EMassMovementAction::Move;

	// Approximate distance to end of path. This distance is the same the AI uses to handle end of path.
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	float DistanceToEndOfPath = 0.0f;

	// What the AI expects to do at the end of the path (i.e. keep on moving, stand).
	UPROPERTY(Transient, EditAnywhere, BlueprintReadOnly)
	EMassMovementAction ActionAtEndOfPath = EMassMovementAction::Move;
};
