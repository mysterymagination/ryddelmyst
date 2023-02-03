// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnowballAttack.h"
#include "IcySnowballAttack.generated.h"

/**
 * An Attack representing the damage formula and configurable effects/damage type of the IcySnowball spell, dealing cold damage and freezing the target
 */
UCLASS()
class RYDDELMYST_API UIcySnowballAttack : public USnowballAttack
{
	GENERATED_BODY()
public:
	UIcySnowballAttack();
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
