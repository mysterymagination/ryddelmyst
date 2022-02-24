// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Describable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDescribable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for objects that have player-visible descriptions
 */
class RYDDELMYST_API IDescribable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Generates and returns a string describing the object 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FString GenerateDescription();
};
