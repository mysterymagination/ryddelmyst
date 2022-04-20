// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "SlippersOfLongWintersNapItem.generated.h"

/**
 * A lovely pair of soft slippers that lull an ice mage's enemies into THE long slumber
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API USlippersOfLongWintersNapItem : public UObject, public IItem
{
	GENERATED_BODY()
	
public:
	/**
	 * Equips the item to the given Actor, granting any passive effects it offers
	 */
	void OnEquip_Implementation(AActor* EquippedActor) override;
};
