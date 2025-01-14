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
protected:
	// Duration during which the effected Actor will be shocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float ShockDuration = 5.f;
public:
	static const FString ATTACK_NAME;
	UElectricSnowballAttack();
	UFUNCTION()
	float GetShockDuration() { return ShockDuration; };
	UFUNCTION()
	void SetShockDuration(float Duration) { ShockDuration = Duration; };
	virtual void OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
