// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "HeadUnit.h"
#include "IAnatomy.h"
#include "IDefender.h"
#include "HeadBoxComponent.generated.h"

/**
 * Form representing the physicality of a creature's head
 */
UCLASS(ClassGroup = "Collision", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Head Collision", BlueprintSpawnableComponent))
class RYDDELMYST_API UHeadBoxComponent : public UBoxComponent, public IAnatomy, public IDefender
{
    GENERATED_BODY()
    
protected:
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UHeadUnit> HeadUnit;
    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UArmor> Armor;

public:
    UHeadBoxComponent();
    UAnatomyUnit* GetAnatomyUnit_Implementation() override;
    UArmor* GetArmor_Implementation() override;
    FBattleStatsData GetBattleStats_Implementation() override;
};
