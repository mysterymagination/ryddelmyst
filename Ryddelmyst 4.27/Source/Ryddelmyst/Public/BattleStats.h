#pragma once

#include "CoreMinimal.h"
#include "BattleStats.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UBattleStats : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TMap<FString, float> StatsMap;

public:
	UBattleStats();
	// todo: create map of stat modifier sources to arrays of pairs of stat name and associated modifier scale factor.
	//  e.g. 
	// {
	//   "leftArmDebilitate" : [
	//     {"attack", 0.5f},
	//     {"magic", 0.25f}
	//   ]
	// }
	// this way we can allow for multiple things to modify stats concurrently and timeout clocks will reverse the modifiers intelligently instead of
	// setting a concrete value irrespective of any unknown modifiers.  The order in which modifiers are applied and removed shouldn't matter since they're
	// discrete operations.

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ScaleStats(float ScaleFactor, AActor* BattleStatsBearer);
	
	UFUNCTION(BlueprintCallable, Category = "RPG")
	FString ToString()
	{
		return FString::Printf(TEXT("{\nAttack: %f,\nDefense: %f,\nSpeed: %f,\nMagic: %f,\nSpirit: %f\n}"), StatsMap["Attack"], StatsMap["Defense"], StatsMap["Speed"], StatsMap["Magic"], StatsMap["Spirit"]);
	}
};