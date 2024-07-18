// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtremityCapsuleComponent.h"
#include "LegCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's leg and processing hits delivered and received via the leg
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Leg Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API ULegCapsuleComponent : public UExtremityCapsuleComponent
{
	GENERATED_BODY()
};
