// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterGenerator.generated.h"

/**
 * A component that spawns monsters parametrically from the approximate location of the owning Actor.
 * Spawning can occur on a clock, by event, or both.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "MonsterGenerator", BlueprintSpawnableComponent))
class RYDDELMYST_API MonsterGenerator : public UActorComponent
{
	GENERATED_BODY()
public:
	MonsterGenerator();
};
