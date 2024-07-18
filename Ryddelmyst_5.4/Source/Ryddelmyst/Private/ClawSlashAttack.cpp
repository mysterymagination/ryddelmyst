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

FAttackTxInfo UClawSlashAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Attack"] * MathUtils::MeanVector3D(BattleStatsData.BattlerTransform.GetScale3D());
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; BasePower (%f) * Attack (%f) = BaseDamage (%f)"), BasePower, BattleStatsData.StatsMap["Attack"], BaseDamage);
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    uint8 DieSides = 8;
    BaseDamage += MathUtils::RollNdM(DieCount, DieSides);
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = BaseDamage / (DieCount * DieSides) >= 0.9f;
    return AttackTx;
}
	