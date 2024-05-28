// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "BurnedStatusEffect.generated.h"

/**
 * Status effect that applies periodic fire damage to the effected Actor
 */
UCLASS()
class RYDDELMYST_API UBurnedStatusEffect : public UStatusEffect
{
	GENERATED_BODY()
	
	/**
	 * @brief Baseline Duration for which the effected Actor should take burn damage e.g. 5 seconds total
	 *
	 */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
		float BaseBurnDuration = 5.f;
	/**
	 * @brief Current Duration for which the effected Actor should take burn damage e.g. 5 seconds total; subject to mutation at runtime.
	 * 
	 */
	UPROPERTY()
	float BurnDuration = BaseBurnDuration;
	/**
	 * @brief Period at which burn damage is applied e.g. effected Actor takes damage every 1 second until duration is met
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float BurnPeriod = 1.f;
	/**
	 * @brief Amount of fire damage the burned Actor takes when burn is applied
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	float BurnDamage = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	FTimerHandle BurnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	FTimerHandle TotalBurnTimerHandle;

	UFUNCTION()
	void Burn(AActor* EffectedActor);
	
public:
	static const FString NAME;

public:
	UBurnedStatusEffect();
	/**
	 * @brief Starts a timer whose function applies fire damage at a set period for a set duration
	 * 
	 * @param EffectedActor the Actor who should be burned
	 */
	void OnEffectApplied(AActor* EffectedActor) override;
	/**
	 * @brief Cancels the BurnTimerHandle, stopping the burn damage application
	 * 
	 * @param EffectedActor the Actor who should no longer be burning
	 */
	void OnEffectRemoved(AActor* EffectedActor) override;
	
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetBurnDuration(float Duration) { BurnDuration = Duration; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetBurnDuration() { return BurnDuration; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetBaseBurnDuration(float Duration) { BaseBurnDuration = Duration; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetBaseBurnDuration() { return BaseBurnDuration; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetBurnPeriod(float Period) { BurnPeriod = Period; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetBurnPeriod() { return BurnPeriod; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetBurnDamage(float Damage) { BurnDamage = Damage; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	float GetBurnDamage() { return BurnDamage; };
	
};
