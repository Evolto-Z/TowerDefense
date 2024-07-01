// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAnimInstance.h"

#include "CrowdActorInterface.h"

void UCrowdAnimInstance::NativeInitializeAnimation()
{
	
}

void UCrowdAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (ICrowdActorInterface* CrowdActor = Cast<ICrowdActorInterface>(GetOwningActor()))
	{
		AnimInstanceData = CrowdActor->GetMutableAnimInstanceData();
	}
}

void UCrowdAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	
}

void UCrowdAnimInstance::NativePostEvaluateAnimation()
{
	
}
