// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtremityCapsuleComponent.h"
#include "IAttacker.h"
#include "ClawWeapon.h"
#include "ClawCapsuleComponent.generated.h"

/**
 * A UPrimitiveComponent representing the physical form of a creature's claw
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Claw Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UClawCapsuleComponent : public UExtremityCapsuleComponent, public IAttacker
{
	GENERATED_BODY()
	UPROPERTY()
	UClawWeapon* Claw;
public:
	UFUNCTION()

};
