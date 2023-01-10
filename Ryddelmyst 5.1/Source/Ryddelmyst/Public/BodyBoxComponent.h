// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "CoreUnit.h"
#include "IAnatomy.h"
#include "IDefender.h"
#include "BodyBoxComponent.generated.h"

/**
 * Form representing the physical core of a creature
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Body Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UBodyBoxComponent : public UBoxComponent, public IAnatomy, public IDefender
{
    GENERATED_BODY()
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UCoreUnit* CoreUnit;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    UArmor* Armor;

public:
    UAnatomyUnit* GetAnatomyUnit_Implementation() override;
    UArmor* GetArmor_Implementation() override;
};
