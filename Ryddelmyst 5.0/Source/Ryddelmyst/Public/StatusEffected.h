// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatusEffect.h"
#include "StatusEffected.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatusEffected : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface applies to any Actor who can be subject to a StatusEffect and must therefore process them.
 */
class RYDDELMYST_API IStatusEffected
{
	GENERATED_BODY()

public:
	/**
	 * @brief Iterates through the effected Actor's StatusEffect container, running StatusEffect::OnEffect() for each
	 * todo: do we need this?  So far StatusEffect objects have been able to get away with being self-driven with timers and we call their OnEffectApplied() as we assign them to effected Actors.
	 * Something like this makes a lot more sense in a fully turn based context, and indeed may be needed when we switch to turn based mode (once that's a thing).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Magic")
	void ProcessStatusEffects();

	/**
	 * @brief Add a given StatusEffect to the Actor's StatusEffect container
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Magic")
	void AddStatusEffect(UStatusEffect* Effect);

	/**
	 * @brief Remove a given StatusEffect from the Actor's StatusEffect container
	 * 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Magic")
	void RemoveStatusEffect(const FString& EffectId);
};
