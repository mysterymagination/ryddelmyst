// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "SnowballAttack.generated.h"

/**
 * An Attack representing the damage formula and configurable effects/damage type of the Snowball spell
 */
UCLASS()
class RYDDELMYST_API USnowballAttack : public UAttack
{
	GENERATED_BODY()
public:
	USnowballAttack();
	// todo: need configurable damage types
	// todo: need configurable effects
};
