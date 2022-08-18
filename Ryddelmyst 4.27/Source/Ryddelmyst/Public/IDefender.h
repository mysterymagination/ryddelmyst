// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

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
	 * @return an array of damage types that this defending object resists
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	const TArray<TSubclassOf<UDamageType>> GetResistances();
	/**
	 * @return the scaling factor applied to resisted damage types, e.g. 0.5f reduces resisted damage by half
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetResistanceFactor();
	/**
	 * @return an array of damage types to which this defending object is vulnerable
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	const TArray<TSubclassOf<UDamageType>> GetVulnerabilities();
	/**
	 * @return the scaling factor applied to vulnerable damage types, e.g. 1.5f increases vulnerable damage to 150%
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetVulnerabilityFactor();
};
