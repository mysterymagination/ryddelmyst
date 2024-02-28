// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBullet.h"
#include "FlameBeadBullet.generated.h"

/**
 * A tiny sphere of fire suitable for bursts of bullets!
 */
UCLASS(ClassGroup = "Combat", Blueprintable, BlueprintType, meta = (DisplayName = "FlameBeadBullet", BlueprintSpawnableComponent))
class RYDDELMYST_API AFlameBeadBullet : public ASpellBullet
{
	GENERATED_BODY()
	
public:
	AFlameBeadBullet();
};
