// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.h"
#include "Describable.generated.h"

USTRUCT(BlueprintType)
struct FLibraryBookData
{
	GENERATED_BODY()
public:
	/**
	 * @brief Quick description of the object, suitable for library book lists.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText LocalizedTitle;
	/**
	 * @brief Long form details about the object, suitable for quest log entry.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText LocalizedLore;
	/**
	 * @brief CoverArtPath string path relative to game assets where the desired library book cover art image resides.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FString CoverArtPath;
};

USTRUCT(BlueprintType)
struct FDescriptor
{
	GENERATED_BODY()
public:
	/**
	 * @brief Quick description of the object, suitable for convo bubble.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText LocalizedDescription;
	/**
	 * @brief Data for the quest log entry.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FLibraryBookData Lore;
	/**
	 * @brief Reaction sprite showing player character's emotional response to the described object.
	 */
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
	 * @param FName BoneName - the bone closest to where our interact physics ray impacted the target, used to modify description based on part of the object being examined; only relevant for skeletalmeshes and will be None by default.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	FDescriptor GenerateDescription(FName BoneName);
};
