#pragma once

#include "CoreMinimal.h"
#include "BattleStats.generated.h"

UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UBattleStats : public UObject
{
	GENERATED_BODY()

public:
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
	// todo: stat map of stat name strings to float values instead of individual variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	uint8 Lvl = 1;
	/**
	 * @brief Determines magic damage dealt to a target before their Spirit or Defense is taken into account
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float Magic = 10.f;
	/**
	 * @brief Determines the reduction in damage taken from magical energy sources
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float Spirit = 10.f;
	/**
	 * @brief Determines physical damage from melee weapons
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float Attack = 10.f;
	/**
	 * @brief Determines the reduction in damage taken from physical sources
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float Defense = 10.f;
	/**
	 * @brief This is a scaling factor for the speed component of a moving Actor's velocity, usually controlled by a variable within its MovementComponent subclass e.g. Max*Speed for CharacterMovementComponent.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float Speed = 1.f;
	/**
	 * @brief This is the character's health points; lte 0 means death.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float HP = 100.f;
	/**
	 * @brief This is the character's mana points; a value gte a spell's cost is required to cast the spell.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="RPG")
	float MP = 100.f;

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ScaleStats(float ScaleFactor, AActor* BattleStatsBearer);
	
	UFUNCTION(BlueprintCallable, Category = "RPG")
	FString ToString()
	{
		return FString::Printf(TEXT("{\nAttack: %f,\nDefense: %f,\nSpeed: %f,\nMagic: %f,\nSpirit: %f\n}"), Attack, Defense, Speed, Magic, Spirit);
	}
};