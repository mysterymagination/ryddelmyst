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

public:
	UFUNCTION(BlueprintCallable, Category = "Magic")
	void OnEffect(AActor* EffectedActor){};

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void SetId(const FString& Id){ this.Id = Id; };

	UFUNCTION(BlueprintCallable, Category = "Magic")
	const FString& GetId(){ return this.Id; };
};
