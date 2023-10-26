// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "CoreUnit.h"
#include "IAnatomy.h"
#include "IDefender.h"
#include "BodyCapsuleComponent.generated.h"

/**
 * Form representing the physical core of a roughly capsuley shaped creature
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Body Capsule Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UBodyCapsuleComponent : public UCapsuleComponent, public IAnatomy, public IDefender
{
	GENERATED_BODY()
protected:
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCoreUnit> CoreUnit;
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArmor> Armor;

public:
    UBodyCapsuleComponent();
    UAnatomyUnit* GetAnatomyUnit_Implementation() override;
    UArmor* GetArmor_Implementation() override;
    AActor* GetBattler_Implementation() override;
};
