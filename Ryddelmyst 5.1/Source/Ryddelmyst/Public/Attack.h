// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Containers/Map.h"
#include "BattleStatsBearer.h"
#include "Attack.generated.h"

/**
 * An Attack represents a particular attack that can be performed with a Weapon; it provides data and logic necessary to calculate damage and deliver additional effects to the stricken target.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Attack", BlueprintSpawnableComponent))
class RYDDELMYST_API UAttack : public UObject
{
	GENERATED_BODY()
public:
	static const FString KEY_COSTS_EFFECT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString AttackName;
	/**
	 * TMap of UDamageType subclass' UClass pointers to a percentage weight float; the weights from all damage types should add up to 1  
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<TSubclassOf<UDamageType>, float> DamageTypesToWeightsMap;
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
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageTx(AActor* BattleStatsBearer);
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) { return 0.f; }
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
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
				float* StatPtr = BattleStats->StatsMap.Find(Cost.Key);
				if(StatPtr)
				{
					*StatPtr -= Cost.Value;
				}
		}
	}
};
