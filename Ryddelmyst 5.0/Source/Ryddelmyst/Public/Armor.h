// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Armor.generated.h"

/**
 * Armor handles incoming hits, calculating the damage the armored creature receives and providing information about resistances and vulnerabilities.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Armor", BlueprintSpawnableComponent))
class RYDDELMYST_API UArmor : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * @brief Calculates the final damage received from an incoming attack
	 * @param BattleStatsBearer the battle stats of the creature wearing this armor
	 * @param BaseDamage the damage the attack is attempting to deliver before armor, resistances, weaknesses etc. are taken into account
	 * @param DamageTypes the types of damage the incoming attack deals
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageRx(AActor* BattleStatsBearer, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes);
	virtual float CalculateDamageRx_Implementation(AActor* BattleStatsBearer, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes) {return 0.f;}
    /**
     * @return the defender's resistance scaling factor for the given damage type, which should reduce the damage if the defender has a resistance to the input damage type and otherwise should be 1 e.g. 0 <= factor <= 1
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetResistance(TSubclassOf<UDamageType> InputDamageType);
	virtual float GetResistance_Implementation(TSubclassOf<UDamageType> InputDamageType) {return 1.f;}
    /**
     * @return the defender's vulnerability scaling factor for the given damage type, which should increase the damage if the defender has a vulnerability to the input damage type and otherwise should be 1 e.g. factor >= 1
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetVulnerability(TSubclassOf<UDamageType> InputDamageType);
	virtual float GetVulnerability_Implementation(TSubclassOf<UDamageType> InputDamageType) {return 1.f;}
};
