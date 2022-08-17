// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IAttacker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIAttacker : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects to provide info about how much damage their attack deals and what type of damage it is
 */
class RYDDELMYST_API IIAttacker
{
	GENERATED_BODY()

public:
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageTx(FString AttackName, AActor* BattleStatsBearer);
	/**
	 * @return the types of damage dealt by this attacking object
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	TArray<TSubclassOf<UDamageType>> GetDamageTypes();
};
