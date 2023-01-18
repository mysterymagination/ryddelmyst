// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnatomyUnit.h"
#include "Armor.generated.h"

UENUM()
enum EDamageCat
{
	Physical	UMETA(DisplayName = "Physical Damage"),
	Magical		UMETA(DisplayName = "Energy Damage")
};


/**
 * Armor handles incoming hits, calculating the damage the armored creature receives and providing information about resistances and vulnerabilities.
 */
UCLASS(ClassGroup = "Combat", EditInlineNew, Blueprintable, BlueprintType, meta = (DisplayName = "Armor"))
class RYDDELMYST_API UArmor : public UObject
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG, meta = (AllowPrivateAccess = "true"))
	float PhysicalDamageReductionFactor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG, meta = (AllowPrivateAccess = "true"))
	float MagicDamageReductionFactor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG, meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<UDamageType>, TEnumAsByte<EDamageCat>> DamageCatMap;  
public:
	UArmor();
	/**
	 * @brief Calculates the final damage received from an incoming attack
	 * @param BattleStatsBearer the battle stats of the creature wearing this armor
	 * @param AnatomyCovered the UAnatomyUnit that this armor (or the stricken area of the armor) covers, effectively.  Actually it is the 
	 * 						 anatomy known to the UPrimitiveComponent intercepting the hit who also owns this UArmor instance and 
	 * 						 will forward on said anatomy to the armor as needed
	 * @param BaseDamage the damage the attack is attempting to deliver before armor, resistances, weaknesses etc. are taken into account
	 * @param DamageTypes the types of damage the incoming attack deals
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageRx(AActor* BattleStatsBearer, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes);
	virtual float CalculateDamageRx_Implementation(AActor* BattleStatsBearer, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes);
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
	/**
     * @return the defender's damage reduction scaling factor for the categorical damage EDamageCat (e.g. phys or mag) that these 
	 * damage types map to.  If multiple EDamageCats are met within this set of damage types, then we look at the weights paired with
	 * each damage type and apply them to the relevant EDamageCat damage reduction factor.  We then recombine these values into a scalar
	 * by adding the weights multiplied by the EDamageCat DR to each other.
	 * 
	 * e.g.
	 * Incoming Damage {
	 * 	Fire: 0.5f,
	 *  Bludgeoning: 0.5f
	 * }
	 * DR {
	 * 	Phys: 1.f,
	 * 	Mag: 0.5f
	 * }
	 * Fire * Mag = 0.25
	 * Bludgeoning * Phys = 0.5
	 * These values represent what percentage of each categorical damage component was negated by DR.
	 * 0.25 + 0.5 = 0.75 meaning total DR is 75% and only 25% damage gets through.
     *
     */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float GetDamageReductionFactorForDamageTypes(const TArray<TSubclassOf<UDamageType>>& InputDamageTypes);
	virtual float GetDamageReductionFactorForDamageTypes_Implementation(const TArray<TSubclassOf<UDamageType>>& InputDamageTypes);
	
};
