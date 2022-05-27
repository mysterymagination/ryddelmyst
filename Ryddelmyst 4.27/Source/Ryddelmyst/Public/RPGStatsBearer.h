// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "RPGStatsBearer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URPGStatsBearer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RYDDELMYST_API IRPGStatsBearer
{
	GENERATED_BODY()

public:
	/**
	 * @return the RPGStats object describing an Actor's RPG capabilities e.g. strength, defense...
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG")
	URPGStats* GetStats();
};
