// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnatomyUnit.h"
#include "HeadUnit.generated.h"

/**
 * A unit of anatomy that typically houses the brain and takes double damage
 */
UCLASS(EditInlineNew, Blueprintable, BlueprintType, meta = (DisplayName = "Head"))
class RYDDELMYST_API UHeadUnit : public UAnatomyUnit
{
	GENERATED_BODY()
	
	// todo: override debilitate and add a confusion effect that randomizes behavior for a time
	
	
};
