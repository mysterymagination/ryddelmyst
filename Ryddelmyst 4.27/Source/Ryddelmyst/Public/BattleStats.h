#include "CoreMinimal.h"

USTRUCT(BlueprintType)
struct FBattleStats 
{
	GENERATED_BODY()
	// todo: map instead of individual variables
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

	UFUNCTION(BlueprintCallable, Category = "RPG")
	void ScaleStats(float ScaleFactor)
	{
		Strength *= ScaleFactor;
		Defense *= ScaleFactor;
		Speed *= ScaleFactor;
		Magic *= ScaleFactor;
		Spirit *= ScaleFactor;
	}
};