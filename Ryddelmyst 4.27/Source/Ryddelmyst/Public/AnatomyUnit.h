// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IAttacker.h"
#include "IDefender.h"
#include "BattleStatsBearer.h"
#include "AnatomyUnit.generated.h"

/**
 * Base class for a unit of an organism's anatomy, such as arms, legs, or head.  Processes incoming damage from attacks based on the specific role of the body part
 * with customization options for different types of creatures.
 */
UCLASS()
class RYDDELMYST_API UAnatomyUnit : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	FString CurrentAttack;
	// todo: do we want to maybe parse the BattleStats StatMods map to see if there's an existing key for our debilitate modifier instead of tracking manually?
	UPROPERTY()
	bool IsDebilitated = false;
	/**
	 * The name of the UPrimitiveComponent subclass representing the form of the anatomy to which this unit of function applies; mostly for logging purposes. 
	 */
	UPROPERTY()
	FString FormName;
	/**
	 * @brief Damage types dealt by this (absolute) unit of anatomy, e.g. Bludgeoning for kicks/stomps or Slashing for talons
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float DebilitateScaleFactor = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float DebilitatePeriod = 5.0f;

public:
	const TArray<TSubclassOf<UDamageType>>& GetDamageTypes() { return DamageTypes; }
	const FString& GetCurrentAttack() { return CurrentAttack; }
private:
	/**
	 * @brief Applies a timed BattleStats modification to the damaged AActor based on the stricken anatomy e.g. a stricken leg may reduce speed by half
	 * @param BattleStatsBearer the AActor implementing IBattleStatsBearer on whom we wish to modify stats; presumably the AActor owner (though maybe indirectly via Components) of this AnatomyUnit
	 */
	UFUNCTION()
	virtual void Debilitate(AActor* BattleStatsBearer){};
};
