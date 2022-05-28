// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "FrozenStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API UFrozenStatusEffect : public UStatusEffect
{
	GENERATED_BODY()

	/**
	 * @brief Duration for which the effected Actor should remain frozen (tick() disabled)
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float FreezeDuration = 1.f;
	UPROPERTY()
	FTimerHandle FreezeTimerHandle;
	
public: 
	UFrozenStatusEffect();
	/**
	 * @brief Disables the EffectedActor's tick() so they are frozen for FreezeDuration seconds
	 * 
	 * @param EffectedActor the Actor who should be frozen
	 */
	void OnEffectApplied(AActor* EffectedActor) override;
	/**
	 * @brief Re-enables the EffectedActor's tick() so they are unfrozen
	 * 
	 * @param EffectedActor the Actor who should be unfrozen
	 */
	void OnEffectRemoved(AActor* EffectedActor) override;
	
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetFreezeDuration(float Duration) { FreezeDuration = Duration; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetFreezeDuration() { return FreezeDuration; };
};
