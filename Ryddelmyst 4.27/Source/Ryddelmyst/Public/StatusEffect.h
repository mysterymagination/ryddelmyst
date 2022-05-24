// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.generated.h"

/**
 * Parent class for all status effects e.g. frozen, burned etc.
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UStatusEffect : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Magic", meta = (AllowPrivateAccess = "true"))
	FString Id;

	// todo: how should duration work between real time and turn based contexts?  I guess the best thing would be to set a time and then define an amount of time that a round of turns represents e.g. 6 seconds like in D&D.  Then you could keep a count of N rounds in turn based mode and consider 6N seconds having expired from the duration at any given point, and duration/6 can be considered the count in rounds for which the effect should persist.  Obviously switching between turn based and real time would need a way to tell the StatusEffect timers to stop running and instead count rounds and vice versa with adjusted remaining time.

public:
	UFUNCTION(BlueprintCallable, Category = "Magic")
	virtual void OnEffectApplied(AActor* EffectedActor){};

	UFUNCTION(BlueprintCallable, Category = "Magic")
	virtual void OnEffectRemoved(AActor* EffectedActor){};

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetId(const FString& EffectId){ Id = EffectId; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	const FString& GetId(){ return Id; };
};
