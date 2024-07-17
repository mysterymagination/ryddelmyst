// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "ShockedStatusEffect.generated.h"

/**
 * Status effect that scales all of the afflicted entity's stats, usually scaling down by half.
 */
UCLASS()
class RYDDELMYST_API UShockedStatusEffect : public UStatusEffect
{
	GENERATED_BODY()

	/**
	 * @brief Duration for which the effected Actor should be shocked
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float ShockDuration = 5.f;
	/**
	 * @brief Scale factor to be applied to all of the effected IRPGStats implementor's stats, including movement speed. 
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float StatScaleFactor = 0.5f;
	UPROPERTY()
	FTimerHandle ShockTimerHandle;
	
public:
	static const FString NAME;

public:	
	UShockedStatusEffect();
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetShockDuration(float Duration) { ShockDuration = Duration; };
	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetShockDuration() { return ShockDuration; };
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetStatScale(float Scale) { StatScaleFactor = Scale; };
	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetStatScale() { return StatScaleFactor; };
	/**
	 * @brief Lowers the effected Actor's stats and slows their movement speed, then starts a timer whose function will remove the shocked effect after the duration expires
	 * 
	 * @param EffectedActor the Actor who should be burned
	 */
	void OnEffectApplied(AActor* EffectedActor) override;
	/**
	 * @brief Restores the effected Actor's stats and movement speed to normal
	 * todo: this paradigm does not support multiple sources modifying stats at the same time... to do that we'd need a mapping between sources of stat mods and then a process by which all stat mod effects are taken into account (per-frame or just as mods are applied/removed?) from each active source and then later removed when a source goes inactive.  Problem for future Jeff
	 * @param EffectedActor the Actor who should no longer be burning
	 */
	void OnEffectRemoved(AActor* EffectedActor) override;
	
};
