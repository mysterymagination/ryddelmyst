// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BreathAttack.h"
#include "WoodfirebreathAttack.generated.h"

/**
 * A breath attack based on the user's current defense and HP. Inflicts burned and shocked status effect. 
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "WoodfirebreathAttack", BlueprintSpawnableComponent))
class RYDDELMYST_API UWoodfirebreathAttack : public UBreathAttack
{
	GENERATED_BODY()
protected:
	// Damage die class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	uint8 DieSides = 8;
	// Duration during which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnDuration = 5.f;
	// Period at which the effected Actor will be burned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float BurnPeriod = 2.5f;
	// Duration during which the effected Actor will be shocked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float ShockDuration = 5.f;
	// Main stat from which the damage derives; expects to be a big stat resource like HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	FString PrimaryStat = TEXT("HP");
	// Secondary stat from which the damage derives; expects to be a small stat resource like Defense
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	FString SecondaryStat = TEXT("Defense");
public:
	UWoodfirebreathAttack();
	virtual FAttackTxInfo CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData) override;
	virtual void OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;

public:
	static const FString ATTACK_NAME;
};
