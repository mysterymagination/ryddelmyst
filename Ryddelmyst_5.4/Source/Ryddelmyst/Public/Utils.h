#pragma once

#include "CoreMinimal.h"
#include "Utils.generated.h"

USTRUCT(BlueprintType)
struct FBattleStatsData
{
	GENERATED_BODY()

	/**
	 * Mapping of RPG statistic names to values.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<FString, float> StatsMap;
	/**
	 * Name of the Actor these stats are attributed to.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString BattlerName;
	/**
	 * Geometry transform associated with the Actor these stats are attributed to; used for e.g. scaling claw slash damage based on Actor scale.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FTransform BattlerTransform;
};