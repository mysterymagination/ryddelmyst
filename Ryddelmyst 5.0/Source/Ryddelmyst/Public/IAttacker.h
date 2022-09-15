// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IAttacker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttacker : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects to provide info about how much damage their attack deals and what type of damage it is
 */
class RYDDELMYST_API IAttacker
{
	GENERATED_BODY()

public:
	/**
	 * @brief Processes a hit by this attacker on the given defender
	 * @param StrikingComp the component doing the hitting; this is the IAttacker impl
	 * @param StrickenActor the AActor we hit
	 * @param StrickenComp the UPrimitiveComponent we hit; this is the IDefender impl
	 * @param NormalImpulse the impulse vector normal to our strike point
	 * @param HitInfo collision data
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageTx(const FString& AttackName, AActor* BattleStatsBearer);
	/**
	 * @param AttackName the string name of the attack for which we wish to query damage types; most attacks will only have one, but they could have multiples so we return an array
	 * @return the types of damage dealt by this attacking object
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	TArray<TSubclassOf<UDamageType>> GetDamageTypes(const FString& AttackName);
};
