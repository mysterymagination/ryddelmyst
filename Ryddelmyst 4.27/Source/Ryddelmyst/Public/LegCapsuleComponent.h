// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "ExtremityUnit.h"
#include "IAttacker.h"
#include "IDefender.h"
#include "LegCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's leg and processing hits delivered and received via the leg
 */
UCLASS()
class RYDDELMYST_API ULegCapsuleComponent : public UCapsuleComponent, public IAttacker, public IDefender
{
	GENERATED_BODY()
	
	UPROPERTY()
	UExtremityUnit* LegUnit;
	
protected:
	/**
	 * @brief Damage types dealt by this leg, e.g. Bludgeoning for kicks/stomps or Slashing for talons
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	
	
};
