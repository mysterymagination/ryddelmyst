// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ClawWeapon.generated.h"

/**
 * A Weapon usually associated with the endpoint of a creature's extremity, providing 
 * slashing attacks
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "ClawWeapon", BlueprintSpawnableComponent))
class RYDDELMYST_API UClawWeapon : public UWeapon
{
	GENERATED_BODY()
public:
	UClawWeapon();
};
