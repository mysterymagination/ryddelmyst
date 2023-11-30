#pragma once

#include "CoreMinimal.h"
#include "Utils.generated.h"

USTRUCT(BlueprintType)
struct FBattleStatsData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TMap<FString, float> StatsMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString BattlerName;
};