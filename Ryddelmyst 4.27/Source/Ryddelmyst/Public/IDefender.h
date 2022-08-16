// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IDefender.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIDefender : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects subject to damage to determine how much actual damage is received after factoring in e.g. armor or non-vital anatomy 
 */
class RYDDELMYST_API IIDefender
{
	GENERATED_BODY()

public:
	/**
	 * @brief Calculates the final damage received from an incoming attack
	 * @param BaseDamage the damage the attack is attempting to deliver before armor, resistances, weaknesses etc. are taken into account
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageRx(float BaseDamage, TSubclassOf<UDamageType> DamageType);
	
};
