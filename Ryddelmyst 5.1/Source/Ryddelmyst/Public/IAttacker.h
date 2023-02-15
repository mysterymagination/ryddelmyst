// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon.h"
#include "IAttacker.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAttacker : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface allowing objects to provide info about how much damage their attack deals and what type of damage it is, and what exactly should happen when the attack connects.
 * UPrimitiveComponents that act as weapon forms implement IAttacker which allows us to lookup the Weapon and send in attack names; that way we can have reusable Weapon/Attack objects with common default behavior logic whose results are modified by parameterized instances, and those can be wired up to whatever UPrimitiveComponents we need. 
 */
class RYDDELMYST_API IAttacker
{
	GENERATED_BODY()

public:
	/**
	 * @return a pointer to the AActor implementing IBattleStatsBearer whose BattleStats should be used when processing outgoing damage 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	AActor* GetBattler();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	UWeapon* GetWeapon();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	FString GetCurrentAttack();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void ExecuteAttack
	(
		const FString& AttackName
	);
};
