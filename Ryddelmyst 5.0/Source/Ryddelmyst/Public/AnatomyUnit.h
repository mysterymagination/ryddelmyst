// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BattleStatsBearer.h"
#include "AnatomyUnit.generated.h"

/**
 * Base class for a unit of an organism's anatomy, such as arms, legs, or head.  Processes incoming attacks based on the specific role of the body part
 * with customization options for different types of creatures.
 */
UCLASS()
class RYDDELMYST_API UAnatomyUnit : public UObject
{
	GENERATED_BODY()
	
protected:
	// todo: do we want to maybe parse the BattleStats StatMods map to see if there's an existing key for our debilitate modifier instead of tracking manually?
	UPROPERTY()
	bool IsDebilitated = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float DebilitateScaleFactor = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RPG)
	float DebilitatePeriod = 5.0f;

public:
	/**
	 * @brief Applies a timed BattleStats modification to the damaged AActor based on the stricken anatomy e.g. a stricken leg may reduce speed by half
	 * @param BattleStatsBearer the AActor implementing IBattleStatsBearer on whom we wish to modify stats; presumably the AActor owner (though maybe indirectly via Components) of this AnatomyUnit
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = RPG)
	void Debilitate(AActor* BattleStatsBearer);
	virtual void Debilitate_Implementation(AActor* BattleStatsBearer){};
};
