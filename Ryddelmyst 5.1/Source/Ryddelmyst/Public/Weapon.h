// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Attack.h"
#include "Weapon.generated.h"

/**
 * An object representing a weapon of some sort, e.g. a sword or claw.  It has a map of Attacks it can perform which include data such as the damage type an attack inflicts, the base damage value etc.
 * Weapons' functionality can then be associated with an appropriate form, e.g. a ClawCapsuleComponent has a ClawWeapon. 
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Weapon", BlueprintSpawnableComponent))
class RYDDELMYST_API UWeapon : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TMap<FString, UAttack*> AttackMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FString CurrentAttackName;
};
