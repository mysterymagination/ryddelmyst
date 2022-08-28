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
	UBattleStats* GetStats();

	/**
	 * @brief performs implementation specific modification to the Actor's stat of the given name e.g. "Speed" modification on an ACharacter subclass would likely do something like CharacterMovementComponent->MaxWalkSpeed *= the updated speed value
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG")
	void HandleStatModification(const FString& StatName);
};
