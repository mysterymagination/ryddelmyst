#pragma once

#include "CoreMinimal.h"
#include <utility>
#include <unordered_map>
#include <string>
#include <vector>
#include "BattleStats.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UBattleStats : public UObject
{
	GENERATED_BODY()

	/**
	 * Mapping of string source names e.g. "anatomy_debilitate_leftArm" to an array of pairings of stat name and associated modifier scale factor.
	 * e.g.
	 * {
	 *   "anatomy_debilitate_leftArm" : [
	 *     {"attack", 0.5f},
	 *     {"magic", 0.25f}
	 *   ]
	 * }
	 *
	 * This mapping allows for multiple things to modify stats concurrently and timeout clocks will reverse the modifiers intelligently (with attribution and multiplying by the inverse of the modifier value) rather than restoring a cached concrete value irrespective of any unknown modifiers.  The order in which modifiers are applied and removed shouldn't matter since they're discrete multiplication operations on scalars.
	 * NOTE: hadta use stdlib stuff because UE4 was being a pain about UPROPing up a TMap of TArray of TPair, so we have to hide from the GC instead... *sigh*
	 */
	std::unordered_map<std::string, std::vector<std::pair<std::string, float>>> StatMods;
public:
	/**
	 * Mapping of string statistic names e.g. "Attack" to corresponding float values that will be used in various gameplay mechanics formulas, such as calculating damage by scaling a base damage value by a given stat value. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TMap<FString, float> StatsMap;

public:
	UBattleStats();

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ScaleStats(float ScaleFactor, AActor* BattleStatsBearer);

	/**
	 * Records a stat modification in StatMods by the effect named in AttributionName.
	 * @param AttributionName the cause of the stat mod
	 * @param StatName the name of the stat we're modifying; must correspond to keys in StatsMap
	 * @param ScaleFactor the float value by which the stat value should be multiplied
	 * @param Duration the time in seconds before this modification reveres itself; a value lte 0 will make the modification permanent
	 * @param BattleStatsBearer the IBattleStatsBearer implementor whose stats are being modified; needed for the UpdateSpeed() impl
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ModifyStatByAttribution(const FString& AttributionName, const FString& StatName, float ScaleFactor, float Duration, AActor* BattleStatsBearer);

	/**
	 * Reverses all stat modifications in StatMods by the effect named in AttributionName, if any, by multiplying the modified stats by the inverse of the value they were scaled by.  Then we remove the attribution name from the statmods map since its modifications were reversed.
	 * @param AttributionName the cause of the stat mod
	 * @param BattleStatsBearer the IBattleStatsBearer implementor whose stats are being modified; needed for the UpdateSpeed() impl
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void UnmodifyStatsByAttribution(const FString& AttributionName, AActor* BattleStatsBearer);

	/**
	 * Resets permanently modifiable stats to max values 
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ResetStats();
	
	UFUNCTION(BlueprintCallable, Category = "RPG")
	FString ToString()
	{
		return FString::Printf(TEXT("{\nAttack: %f,\nDefense: %f,\nSpeed: %f,\nMagic: %f,\nSpirit: %f,\nHP: %f,\nMP: %f\nMaxHP: %f,\nMaxMP: %f\n}"), StatsMap["Attack"], StatsMap["Defense"], StatsMap["Speed"], StatsMap["Magic"], StatsMap["Spirit"], StatsMap["HP"], StatsMap["MP"], StatsMap["MaxHP"], StatsMap["MaxMP"]);
	}
};