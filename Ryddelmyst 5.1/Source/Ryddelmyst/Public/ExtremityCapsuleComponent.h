// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Containers/Map.h"
#include "ExtremityUnit.h"
#include "IAnatomy.h"
#include "IDefender.h"
#include "ExtremityCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's extremity (e.g. arm or leg) and processing hits delivered and received via that extremity
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Extremity Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UExtremityCapsuleComponent : public UCapsuleComponent, public IAnatomy, public IDefender
{
    GENERATED_BODY()
    
protected:
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UExtremityUnit> ExUnit;
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArmor> Armor;

public:
    UAnatomyUnit* GetAnatomyUnit_Implementation() override;
    UArmor* GetArmor_Implementation() override;
};
