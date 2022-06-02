// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleStats.h"
#include "BattleStatsBearer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBattleStatsBearer : public UInterface
{
	GENERATED_BODY()
};

/**
 * An Actor with BattleStats is one who can participate in RPG combat and provide info to support same
 */
class RYDDELMYST_API IBattleStatsBearer
{
	GENERATED_BODY()

public:
	/**
	 * @return the BattleStats object describing an Actor's RPG capabilities e.g. strength, defense...
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG")
	UBattleStats& GetStats();
};
