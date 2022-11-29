// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Containers/Map.h"
#include "ExtremityUnit.h"
#include "IAnatomy.h"
#include "IAttacker.h"
#include "IDefender.h"
#include "ExtremityCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's extremity (e.g. arm or leg) and processing hits delivered and received via that extremity
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Arm Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UExtremityCapsuleComponent : public UCapsuleComponent, public IAnatomy, public IDefender
{
    GENERATED_BODY()
    
    
protected:
    UPROPERTY()
    UExtremityUnit* ExUnit;
    UPROPERTY()
    TMap<TSubclassOf<UDamageType>, float> ResistanceMap;
    UPROPERTY()
    TMap<TSubclassOf<UDamageType>, float> VulnerabilityMap;

public:
    UAnatomyUnit* GetAnatomyUnit_Implementation() {return ExUnit;}
    float CalculateDamageRx_Implementation(float BaseDamage, const TArray<TSubclassOf<UDamageType>>& IncomingDamageTypes) { return BaseDamage; }
    float GetResistance(TSubclassOf<UDamageType> InputDamageType)
    {
        float* ResScalingFactor = ResistanceMap.Find(InputDamageType);
        if(ResScalingFactor)
        {
            return *ResScalingFactor;
            
        }
        else
        {
            return 1.f;
        }
        
    }
    float GetVulnerability(TSubclassOf<UDamageType> InputDamageType)
    {
        float* VulScalingFactor = VulnerabilityMap.Find(InputDamageType);
        if(VulScalingFactor)
        {
            return *VulScalingFactor;
            
        }
        else
        {
            return 1.f;
        }
    }
};
