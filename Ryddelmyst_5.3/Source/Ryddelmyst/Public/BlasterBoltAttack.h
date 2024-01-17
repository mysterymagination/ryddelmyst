// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "BlasterBoltAttack.generated.h"

/**
 * Inflicts piercing fire damage, ignoring target iframes.
 */
UCLASS()
class RYDDELMYST_API UBlasterBoltAttack : public UAttack
{
	GENERATED_BODY()
public:
	UBlasterBoltAttack();
	virtual FAttackTxInfo CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData);

public:
	static const FString ATTACK_NAME;
	
};
