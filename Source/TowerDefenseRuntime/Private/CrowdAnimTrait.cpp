// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAnimTrait.h"

#include "MassEntityTemplateRegistry.h"

void UCrowdAnimTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FCrowdAnimFragment>();
}
