#pragma once

#include "CoreMinimal.h"
#include "BattleStats.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UBattleStats : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Mapping of string statistic names e.g. "Attack" to corresponding float values that will be used in various gameplay mechanics formulas, such as calculating damage by scaling a base damage value by a given stat value. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TMap<FString, float> StatsMap;
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
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TMap<FString, TArray<TPair<FString, float>>> StatMods;

public:
	UBattleStats();

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ScaleStats(float ScaleFactor, AActor* BattleStatsBearer);
	
	UFUNCTION(BlueprintCallable, Category = "RPG")
	FString ToString()
	{
		return FString::Printf(TEXT("{\nAttack: %f,\nDefense: %f,\nSpeed: %f,\nMagic: %f,\nSpirit: %f\n}"), StatsMap["Attack"], StatsMap["Defense"], StatsMap["Speed"], StatsMap["Magic"], StatsMap["Spirit"]);
	}
};