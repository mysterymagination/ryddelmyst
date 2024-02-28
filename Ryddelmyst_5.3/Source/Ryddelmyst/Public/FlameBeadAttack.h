// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "FlameBeadAttack.generated.h"

/**
 * Delivers minor fire damage with high crit rate and burns the target. Each hit extends the burning duration rather than resetting it, up to the number of sides on the damage die times the original duration of the effect.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "FlameBeadAttack", BlueprintSpawnableComponent))
class RYDDELMYST_API UFlameBeadAttack : public UAttack
{
	GENERATED_BODY()
protected:
	// Damage die class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	uint8 DieSides = 6;
	// Duration during which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnDuration = 5.f;
	// Period at which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnPeriod = 2.5f;
public:
	UFlameBeadAttack();
	virtual FAttackTxInfo CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData);
	virtual void OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;

public:
	static const FString ATTACK_NAME;
};
