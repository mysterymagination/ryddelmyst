// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IAttacker.h"
#include "IDefender.h"
#include "AnatomyUnit.generated.h"

/**
 * Base class for a unit of an organism's anatomy, such as arms, legs, or head.  Processes incoming damage from attacks based on the specific role of the body part
 * with customization options for different types of creatures.
 */
UCLASS()
class RYDDELMYST_API UAnatomyUnit : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	FString CurrentAttack;
	
protected:
	/**
	 * @brief Damage types dealt by this (absolute) unit of anatomy, e.g. Bludgeoning for kicks/stomps or Slashing for talons
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;

public:
	const TArray<TSubclassOf<UDamageType>>& GetDamageTypes() { return DamageTypes; }
	const FString& GetCurrentAttack() { return CurrentAttack; }
private:
	/**
	 * @brief Applies a timed BattleStats modification to the damaged AActor based on the stricken anatomy e.g. a stricken leg may reduce speed by half
	 * 
	 */
	UFUNCTION()
	void Debilitate(){};
};
