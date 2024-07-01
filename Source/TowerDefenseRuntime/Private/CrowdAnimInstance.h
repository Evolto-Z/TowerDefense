// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdAnimInstanceData.h"
#include "Animation/AnimInstance.h"
#include "CrowdAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSERUNTIME_API UCrowdAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FCrowdAnimInstanceData AnimInstanceData;
};
