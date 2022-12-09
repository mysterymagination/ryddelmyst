// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Map.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AnatomyUnit.h"
#include "IDefender.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDefender : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects subject to damage to determine how much actual damage is received after factoring in e.g. armor or non-vital anatomy 
 */
class RYDDELMYST_API IDefender
{
	GENERATED_BODY()

public:
	/**
	 * @brief Calculates the final damage received from an incoming attack
	 * @param BaseDamage the damage the attack is attempting to deliver before armor, resistances, weaknesses etc. are taken into account
	 * @param DamageTypes the types of damage the incoming attack deals
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageRx(float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes);
    /**
     * @return the defender's resistance scaling factor for the given damage type, which should reduce the damage if the defender has a resistance to the input damage type and otherwise should be 1 e.g. 0 <= factor <= 1
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetResistance(TSubclassOf<UDamageType> InputDamageType);
    /**
     * @return the defender's vulnerability scaling factor for the given damage type, which should increase the damage if the defender has a vulnerability to the input damage type and otherwise should be 1 e.g. factor >= 1
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetVulnerability(TSubclassOf<UDamageType> InputDamageType);
};
