// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BreathAttack.h"
#include "WoodfirebreathAttack.generated.h"

/**
 * A breath attack based on the user's current defense and HP. Inflicts burned and shocked status effect. 
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "WoodfirebreathAttack", BlueprintSpawnableComponent))
class RYDDELMYST_API UWoodfirebreathAttack : public UBreathAttack
{
	GENERATED_BODY()
	
};
