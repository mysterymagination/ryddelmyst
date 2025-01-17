// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowballAttack.h"
#include "MathUtils.h"

USnowballAttack::USnowballAttack()
{
    Costs = 
    {
        {
            "MP", 10.f
        }
    };
}

void USnowballAttack::OnHit_Implementation(FBattleStatsData BattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    UE_LOG(LogTemp, Warning, TEXT("SnowballAttack::OnHit; HitComp says %s, OtherActor says %s, OtherComp says %s, normal impulse says %s, hitres says %s"), 
		*StrikingComp->GetName(), *StrickenActor->GetName(), *StrickenComp->GetName(), *NormalImpulse.ToString(), *HitInfo.ToString());
	UE_LOG(LogTemp, Warning, TEXT("SnowballAttack::OnHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
    Super::OnHit_Implementation(BattlerData, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // auxiliary effect lambdas that Snowball spell can apply
	for (auto Effect : EffectsOnTarget)
	{
		Effect(StrickenActor, HitInfo);
	}
}

FAttackTxInfo USnowballAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    // todo: roll this into UAttack.cpp boilerplate and add UPROPERTYs for the params e.g. diesides and crit range
    // todo: crits should be based on small vital hitboxes on targets that if hit always result in a crit, and crit behavior should be defined in a BlueNativeEvent function per UAttack.
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Magic"]; 
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    uint8 DieSides = 6;
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx; Power (%f) * Magic (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, BattleStatsData.StatsMap["Magic"], BaseDamage, Rando);
    BaseDamage += Rando;
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = Rando / static_cast<float>((DieCount * DieSides)) >= 0.9f;
    // add on a full diceroll of damage for crit
    if (AttackTx.IsCrit)
    {
        AttackTx.DamageTx += DieCount * DieSides;
    }
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx; IsCrit is %d based on rando %f div max rando %f which is %f"), AttackTx.IsCrit, Rando, static_cast<float>(DieCount * DieSides), Rando / static_cast<float>((DieCount * DieSides)));
    return AttackTx;
}