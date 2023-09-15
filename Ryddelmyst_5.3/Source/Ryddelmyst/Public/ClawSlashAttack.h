// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "ClawSlashAttack.generated.h"

/**
 * Basic claw attack dealing medium slashing damage
 */
UCLASS()
class RYDDELMYST_API UClawSlashAttack : public UAttack
{
	GENERATED_BODY()
public:
	static const FString ATTACK_NAME;
public:
	UClawSlashAttack();
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) override;
};
