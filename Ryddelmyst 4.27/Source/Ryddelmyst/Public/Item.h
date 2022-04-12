// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Item.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (Blueprintable))
class UItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for Actors that can be added to the player's inventory and used from there by the player
 */
class RYDDELMYST_API IItem
{
	GENERATED_BODY()

public:
	/**
	 * Function run when the player first picks up the item from the stage and places it in their inventory
	 * @param Picker the Actor picking up the Item
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	void OnPickup(AActor* Picker);

	/**
	 * Function run when the player drops the item from inventory back into the stage
	 * @param Dropper the Actor dropping the Item
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	void OnDrop(AActor* Dropper);

	/**
	 * Function run when the player uses the item from their inventory
	 * @param User the Actor using the Item
	 * @return true if the item was used successfully, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	bool OnUse(AActor* User);

	/**
	 * Returns the icon that should represent this item in the player's inventory UI
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	UTexture2D* GetDisplayIcon();

	/**
	 * Equips the item to the given Actor, granting any passive effects it offers
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	void OnEquip(AActor* EquippedActor);

	/**
	 * @return true if the item can be equipped, false otherwise
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	bool IsEquippable() const;

	/**
	 * @return the appropriate equipment slot key as a string, e.g. 'head' for a helmet
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Item")
	FString GetEquipSlot() const;
};
