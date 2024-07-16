// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "BreathAttack.generated.h"

/**
 * An attack that deals damage based on one or more of a creature's stats,
 * usually HP, and ignores any defenses of the target.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "BreathAttack", BlueprintSpawnableComponent))
class RYDDELMYST_API UBreathAttack : public UAttack
{
	GENERATED_BODY()

};
