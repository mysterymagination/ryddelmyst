// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include <string>
#include "DiademHellfireMightItem.generated.h"

/**
 * A mighty tiara that powers up fire spells with extra damage output and knockback
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UDiademHellfireMightItem : public UObject, public IItem
{
	GENERATED_BODY()
	
public:
	static const std::string ID;
	/**
	 * Equips the item to the given Actor, granting any passive effects it offers
	 */
	void OnEquip_Implementation(AActor* EquippedActor) override;
};
