// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterBoltAttack.h"
#include "FireDamageType.h"
#include "MathUtils.h"

const FString UBlasterBoltAttack::ATTACK_NAME("BlasterBoltAttack");

UBlasterBoltAttack::UBlasterBoltAttack()
{
	DamageTypesToWeightsMap =
	{
		{UFireDamageType::StaticClass(), 1.f}
	};
	AttackName = ATTACK_NAME;
	BasePower = 10.f;
}

FAttackTxInfo UBlasterBoltAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Magic"];
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    uint8 DieSides = 4;
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx blaster bolt atk; Power (%f) * Magic (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, BattleStatsData.StatsMap["Magic"], BaseDamage, Rando);
    BaseDamage += Rando;
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = Rando / static_cast<float>((DieCount * DieSides)) >= 0.8f;
    // add on a full diceroll of damage for crit
    if (AttackTx.IsCrit)
    {
        AttackTx.DamageTx += DieCount * DieSides;
    }
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx; IsCrit is %d based on rando %f div max rando %f which is %f"), AttackTx.IsCrit, Rando, static_cast<float>(DieCount * DieSides), Rando / static_cast<float>((DieCount * DieSides)));
    return AttackTx;
}


