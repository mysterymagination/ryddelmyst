// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.h"
#include "Describable.generated.h"

USTRUCT(BlueprintType)
struct FDescriptor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText LocalizedDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	InteractReactions Reaction;
};

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
	 * Generates and returns a descriptor containing a localized text string describing the object as well as a reaction enum indicating which character portrait should be used 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FDescriptor GenerateDescription();
};
