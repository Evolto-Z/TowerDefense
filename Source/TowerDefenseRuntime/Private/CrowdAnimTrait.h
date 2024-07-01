// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassEntityTypes.h"
#include "CrowdAnimTrait.generated.h"

class UAnimToTextureDataAsset;

USTRUCT()
struct FCrowdAnimFragment : public FMassFragment
{
	GENERATED_BODY()

	TWeakObjectPtr<UAnimToTextureDataAsset> AnimToTextureData;
	float GlobalStartTime = 0.f;
	float PlayRate = 1.f;
	int32 AnimationStateIndex = 0;
	bool bSwappedThisFrame = false;
};

/**
 * 
 */
UCLASS()
class TOWERDEFENSERUNTIME_API UCrowdAnimTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
