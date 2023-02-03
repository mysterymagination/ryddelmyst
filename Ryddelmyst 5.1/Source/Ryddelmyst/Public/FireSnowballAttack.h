// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SnowballAttack.h"
#include "FireSnowballAttack.generated.h"

/**
 * An Attack representing the damage formula and configurable effects/damage type of the FireSnowball spell, dealing fire damage and burning the target
 */
UCLASS()
class RYDDELMYST_API UFireSnowballAttack : public USnowballAttack
{
	GENERATED_BODY()
public:
	UFireSnowballAttack();
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
