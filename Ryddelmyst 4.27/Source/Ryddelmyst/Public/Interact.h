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
	GRABBABLE UMETA(DisplayName = "GRABBABLE"),
	PULLABLE UMETA(DisplayName = "PULLABLE"),
	PUSHABLE UMETA(DisplayName = "PUSHABLE"),
	SWITCHABLE UMETA(DisplayName = "SWITCHABLE"),
	HUGGABLE UMETA(DisplayName = "HUGGABLE"),
	DESCRIBABLE UMETA(DisplayName = "DESCRIBABLE"),
	POCKETABLE UMETA(DisplayName = "POCKETABLE"),
	OPENABLE UMETA(DisplayName = "OPENABLE")
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
