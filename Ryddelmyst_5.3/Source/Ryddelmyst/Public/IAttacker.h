// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapon.h"
#include "Utils.h"
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
	 * @return the stats map from the BattleStats of whoever instigated this attack; should be used when processing outgoing damage 
	 * todo: the GetBattleStats() function should be returning battlestats data rather than an Actor who implements IBattleStatsBearer; that way we
	 * wouldn't need to worry about a potential disconnect between the lifetime of the IAttacker and the Actor who is its relevant IBattleStatsBearer. 
	 * In the case of projectiles or other remote attack delivery mechanisms, the IAttacker has no hold on the lifetime of the Actor IBattleStatsBearer, nor should it, so in that case we now rely on IsValid(StrikingBattlerActor) causing the Attack::OnHit() to skip its actions. For now it should be sufficient to pass in the BattleState::StatsMap Map object by value and just work with that. If we ever need any of the stat mod timers etc. that UBattleStats provides we could always write a deep clone function for it.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	FBattleStatsData GetBattleStats();
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
