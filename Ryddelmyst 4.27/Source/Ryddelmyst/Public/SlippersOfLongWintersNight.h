// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "SlippersOfLongWintersNight.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API USlippersOfLongWintersNight : public UObject, public IItem
{
	GENERATED_BODY()
	
public:
	/**
	 * Equips the item to the given Actor, granting any passive effects it offers
	 */
	void OnEquip_Implementation(AActor* EquippedActor) override;
};
