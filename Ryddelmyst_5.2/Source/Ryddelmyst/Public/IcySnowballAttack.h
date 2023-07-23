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
protected:
	// Duration during which the effected Actor will be unable to move
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float FreezeDuration = 5.f;	
public:
	static const FString ATTACK_NAME;
	UIcySnowballAttack();
	UFUNCTION()
	float GetFreezeDuration() { return FreezeDuration; };
	UFUNCTION()
	void SetFreezeDuration(float Duration) { FreezeDuration = Duration; };	
	virtual void OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
