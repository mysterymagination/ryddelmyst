// Fill out your copyright notice in the Description page of Project Settings.

#include "ClawSlashAttack.h"
#include "SlashingDamageType.h"
#include "MathUtils.h"

const FString UClawSlashAttack::ATTACK_NAME("Claw Slash Attack");

UClawSlashAttack::UClawSlashAttack()
{
    AttackName = ATTACK_NAME;
    BasePower = 10.f;
    DamageTypesToWeightsMap = 
    {
        {USlashingDamageType::StaticClass(), 1.f}
    };
}

float UClawSlashAttack::CalculateDamageTx_Implementation(AActor* BattleStatsBearer)
{
    float BaseDamage = BasePower * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Attack"] * MathUtils::MeanVector3D(BattleStatsBearer->GetActorScale());
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; BasePower (%f) * Attack (%f) = BaseDamage (%f)"), BasePower, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Attack"], BaseDamage);
	BaseDamage += MathUtils::RollNdM(IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Level"], 8);
	return DamageScaleFactor * BaseDamage;
}
	