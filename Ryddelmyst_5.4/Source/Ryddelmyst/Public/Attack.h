// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Map.h"
#include "BattleStatsBearer.h"
#include "Utils.h"
#include "Attack.generated.h"

USTRUCT(BlueprintType)
struct FAttackTxInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageTx = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	bool IsCrit = false;
};

/**
 * An Attack represents a particular attack that can be performed with a Weapon; it provides data and logic necessary to calculate damage and deliver additional effects to the stricken target.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Attack", BlueprintSpawnableComponent))
class RYDDELMYST_API UAttack : public UObject
{
	GENERATED_BODY()
public:
	static const FString KEY_COSTS_EFFECT;
	/**
	 * Tag string to be applied to this Attack indicating that stricken IDefender's defenses and armor should be ignored.
	 */
	static const FString TAG_FLAG_IGNORE_DEFENSE;
	/**
	 * Tag string to be applied to this Attack's host Actor indicating that stricken Actors' iframes (if any) should be ignored.
	 */
	static const FString TAG_FLAG_IGNORE_IFRAMES;
	/**
	 * Tag string to be applied to this Attack's host Actor indicating that it imposes its own knockback and therefore standard knockback should be skipped.
	 */
	static const FString TAG_FLAG_CUSTOM_KNOCKBACK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString AttackName;
	/**
	 * TMap of UDamageType subclass' UClass pointers to a percentage weight float; the weights from all damage types should add up to 1  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<TSubclassOf<UDamageType>, float> DamageTypesToWeightsMap;
	/**
	 * @brief base power of the attack, used to derive the final damage to be transmitted
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BasePower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageScaleFactor = 1.f;
	/**
	 * A mapping of costs to the user of the attack, expressed as stat name paired with a float value e.g. "HP",15.f to say that an attack costs the 
	 * attacker 15 HP to use. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<FString, float> Costs;
	/**
	 * Array of tag strings that modify attack behavior. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FString> Tags;
	
protected:
	/**
	 * @brief cache of the most recent damage this attack dealt
	 * 
	 */
	UPROPERTY()
	float LatestDamageDealt = 0.f;

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
	void OnHit(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	virtual void OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsData the stats of the instigator of the attack, which determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	FAttackTxInfo CalculateDamageTx(FBattleStatsData BattleStatsData);
	virtual FAttackTxInfo CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData) { return FAttackTxInfo(); }
	/**
	 * @brief Subtracts the costs of the attack from the attacker's resources e.g. MP cost to cast a spell.  This function can't be called from a generic location
	 * 	      within the Attack impl since different attacks will incur costs differently i.e. casting a spell that summons a bullet will incur costs when cast, not
	 * 		  when/if the bullet hits whereas some others may not have any applicable hit chance.  
	 * @param BattleStatsBearer the instigator of the attack, whose stats will incur the costs
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void ProcessCosts(AActor* BattleStatsBearer);
	virtual void ProcessCosts_Implementation(AActor* BattleStatsBearer) 
	{  
		for (auto& Cost : Costs)
		{
			/* todo: a mapping of costs that can be either simple scalar floats or a lambda effect to execute would be cool
			    e.g. the caster flies back N meters or their HP is halved for N seconds; 
				that would have to work outside the blueprint ecosystem tho I think since blueprints don't really support lambda functions.
			if(Cost.Key == KEY_COSTS_EFFECT)
			{
				Cost.Value();
			}
			else
			{
				UBattleStats* BattleStats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
				float* StatPtr = BattleStats->StatsMap.Find(Cost.Key);
				if(StatPtr)
				{
					*StatPtr -= Cost.Value;
				}
			}
			*/
			UBattleStats* BattleStats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
				float* StatPtr = BattleStats->StatsData.StatsMap.Find(Cost.Key);
				if(StatPtr)
				{
					*StatPtr -= Cost.Value;
				}
		}
	}
	/**
	 * Checks that the given BattleStatsBearer can afford the costs of this Attack  
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CheckCosts(AActor* BattleStatsBearer)
	{
		bool Affordable = true;
		for (auto& Cost : Costs)
		{
			UBattleStats* BattleStats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
			float* StatPtr = BattleStats->StatsData.StatsMap.Find(Cost.Key);
			if (StatPtr)
			{
				Affordable &= *StatPtr >= Cost.Value;
			}
		}
		return Affordable;
	}
};
