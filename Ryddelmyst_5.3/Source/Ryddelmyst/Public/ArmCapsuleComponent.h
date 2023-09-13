// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExtremityCapsuleComponent.h"
#include "ArmCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's arm and processing hits delivered and received via the arm
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Arm Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UArmCapsuleComponent : public UExtremityCapsuleComponent
{
    GENERATED_BODY()
};
