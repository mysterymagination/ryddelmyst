// Fill out your copyright notice in the Description page of Project Settings.


#include "BoulderAttack.h"
#include "BludgeoningDamageType.h"

const FString UBoulderAttack::ATTACK_NAME("BoulderAttack");

UBoulderAttack::UBoulderAttack()
{
    DamageTypesToWeightsMap =
    {
        {UBludgeoningDamageType::StaticClass(), 1.f}
    };
    AttackName = ATTACK_NAME;
    BasePower = 1.f;
}

void UBoulderAttack::OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    Super::OnHit_Implementation(StrikingBattlerData, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // todo: stunned StatusEffect?
}

FAttackTxInfo UBoulderAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    // todo: factor in boulder mass somehow; maybe thresholds set the die sides? Maybe the mass itself is the base power?
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Attack"];
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx boulder atk; Power (%f) * Magic (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, BattleStatsData.StatsMap["Magic"], BaseDamage, Rando);
    BaseDamage += Rando;
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = Rando / static_cast<float>((DieCount * DieSides)) >= 0.25f;
    // add on a full diceroll of damage for crit
    if (AttackTx.IsCrit)
    {
        AttackTx.DamageTx += DieCount * DieSides;
    }
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx boulder atk; IsCrit is %d based on rando %f div max rando %f which is %f"), AttackTx.IsCrit, Rando, static_cast<float>(DieCount * DieSides), Rando / static_cast<float>((DieCount * DieSides)));
    return AttackTx;
}