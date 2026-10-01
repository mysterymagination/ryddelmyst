// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "GameFramework/Character.h"
#include "BoulderAttack.generated.h"

/**
 * An attack of bludgeoning damage from a giant hurled stone. Focus is on mass-derived damage and increased knockback.
 */
UCLASS()
class RYDDELMYST_API UBoulderAttack : public UAttack
{
	GENERATED_BODY()
public:
	// Mass of the boulder, which contributes to damage calculation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float Mass = 10;
protected:
	// Damage die class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	uint8 DieSides = 12;
	// todo: stun status effect that flips the camera view to first person suddenly looking up at the stars?
public:
	UBoulderAttack();
	virtual FAttackTxInfo CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData);
	virtual void OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;

public:
	static const FString ATTACK_NAME;
};
