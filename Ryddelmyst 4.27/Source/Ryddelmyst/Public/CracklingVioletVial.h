// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include <string>
#include "CracklingVioletVial.generated.h"

/**
 * Secured to a glittering gold chain is a bubbling vial of liquid lightning with violet arcs constantly striking at their confinement as they search endlessly for ground!  This necklace turns the already mighty ElectricSnowball into a spreadshot.
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UCracklingVioletVial : public UObject, public IItem
{
	GENERATED_BODY()

public:
	static const std::string ID;
	/**
	 * Equips the item to the given Actor, granting any passive effects it offers
	 */
	void OnEquip_Implementation(AActor* EquippedActor) override;
};