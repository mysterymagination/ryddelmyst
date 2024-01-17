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
protected:
	// Duration during which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnDuration = 5.f;
	// Period at which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnPeriod = 2.5f;
public:
	static const FString ATTACK_NAME;
	UFireSnowballAttack();
	UFUNCTION()
	float GetBurnDuration() { return BurnDuration; };
	UFUNCTION()
	void SetBurnDuration(float Duration) { BurnDuration = Duration; };
	UFUNCTION()
	float GetBurnPeriod() { return BurnPeriod; };
	UFUNCTION()
	void SetBurnPeriod(float Period) { BurnPeriod = Period; };
	virtual void OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
};
