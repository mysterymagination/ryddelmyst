// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MagicWeapon.generated.h"

/**
 * A Weapon that delivers spell effects through its Attacks
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "MagicWeapon", BlueprintSpawnableComponent))
class RYDDELMYST_API UMagicWeapon : public UWeapon
{
	GENERATED_BODY()
	
};
