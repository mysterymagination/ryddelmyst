// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteract : public UInterface
{
	GENERATED_BODY()
};

/**
 * Enums tagging different possible ways with which the object can be interacted by players e.g. a grabbable object might let the player know that it should play its grab animation, transform the grabbed object to the appropriate carry position, and attach the carried object to the player transforms.
 */
UENUM(BlueprintType)
enum class InteractCapability : uint8 
{
	/**
	 * @brief indicates that this actor can be picked up physically by the player and carried, but not pocketed
	 */
	GRABBABLE UMETA(DisplayName = "GRABBABLE"),
	/**
	 * @brief indicates that this actor can be pulled about
	 */
	PULLABLE UMETA(DisplayName = "PULLABLE"),
	/**
	 * @brief indicates that this actor can be pushed about
	 */
	PUSHABLE UMETA(DisplayName = "PUSHABLE"),
	/**
	 * @brief indicates that this actor acts as a switch for something and has a binary state
	 */
	SWITCHABLE UMETA(DisplayName = "SWITCHABLE"),
	/**
	 * @brief wishful thinking, non-functional
	 */
	HUGGABLE UMETA(DisplayName = "HUGGABLE"),
	/**
	 * @brief indicates that this actor generates a description for the player character to read out
	 */
	DESCRIBABLE UMETA(DisplayName = "DESCRIBABLE"),
	/**
	 * @brief indicates that this actor contains item data that should be transferred to player inventory
	 */
	POCKETABLE UMETA(DisplayName = "POCKETABLE"),
	/**
	 * @brief indicates that this actor is a container that can be opened
	 */
	OPENABLE UMETA(DisplayName = "OPENABLE"),
	/**
	 * @brief indicates that this actor dispense lore data in its description that should be recorded in player quest library
	 */
	LOREABLE UMETA(DisplayName = "LOREABLE"),
	/**
	 * @brief indicates that this actor can be conversed with
	 */
	TALKABLE UMETA(DisplayName = "TALKABLE"),
	/**
	 * @brief indicates that this actor has been used up upon interaction
	 */
	CONSUMABLE UMETA(DisplayName = "CONSUMABLE")
};

UENUM(BlueprintType)
enum class InteractReactions : uint8 
{
	HAPPY UMETA(DisplayName = "HAPPY"),
	FLIRTY UMETA(DisplayName = "FLIRTY"),
	NEUTRAL UMETA(DisplayName = "NEUTRAL"),
	ANGRY UMETA(DisplayName = "ANGRY"),
	EMBARRASSED UMETA(DisplayName = "EMBARRASSED"),
	CONFUSED UMETA(DisplayName = "CONFUSED"),
	SAD UMETA(DisplayName = "SAD"),
	WEARY UMETA(DisplayName = "WEARY"),
	ELDRITCH UMETA(DisplayName = "ELDRITCH")
};

/**
 * Interface for Actors that the player can interact with in special ways not covered by physics etc.
 */
class RYDDELMYST_API IInteract
{
	GENERATED_BODY()
	
public:
	/**
	 * Performs the interactable object's automatic interact behavior e.g. a red hot object burns the player on interaction.
	 * @param Interactor the Actor who instigated the interaction with the interactable object
	 * @return an InteractCapability array informing the player about how the object can be interacted with.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	TArray<InteractCapability> OnInteract(AActor* Interactor);
};
