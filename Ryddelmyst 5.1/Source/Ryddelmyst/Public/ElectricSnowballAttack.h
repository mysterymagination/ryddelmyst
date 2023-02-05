// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnowballAttack.h"
#include "ElectricSnowballAttack.generated.h"

/**
 * An Attack representing the damage formula and configurable effects/damage type of the ElectricSnowball(!) spell, dealing lightning damage and shocking the target
 */
UCLASS()
class RYDDELMYST_API UElectricSnowballAttack : public USnowballAttack
{
	GENERATED_BODY()
public:
	UElectricSnowballAttack();
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
