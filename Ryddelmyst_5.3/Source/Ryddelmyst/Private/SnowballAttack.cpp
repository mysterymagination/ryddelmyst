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

void USnowballAttack::OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    UE_LOG(LogTemp, Warning, TEXT("SnowballAttack::OnHit; HitComp says %s, OtherActor says %s, OtherComp says %s, normal impulse says %s, hitres says %s"), 
		*StrikingComp->GetName(), *StrickenActor->GetName(), *StrickenComp->GetName(), *NormalImpulse.ToString(), *HitInfo.ToString());
	UE_LOG(LogTemp, Warning, TEXT("SnowballAttack::OnHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
    Super::OnHit_Implementation(StrikingBattler, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // auxiliary effect lambdas that Snowball spell can apply
	for (auto Effect : EffectsOnTarget)
	{
		Effect(StrickenActor, HitInfo);
	}
    // todo: would be cleaner to have this logic in the Snowball Actor itself; maybe by registering a separate OnHit or similar function delegate with the 
    //  same OnComponentHit event that got us here?  Is there any guarantee of the order in which delegate functions are called by a triggering event, I wonder?
    // after expending its Attack, the Snowball Actor hierarchy is destroyed
    StrikingComp->GetOwner()->Destroy();
}

FAttackTxInfo USnowballAttack::CalculateDamageTx_Implementation(AActor* BattleStatsBearer)
{
    if (BattleStatsBearer->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
    {
        float BaseDamage = BasePower * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"];
        
        uint8 DieCount = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Level"];
        uint8 DieSides = 6;
        float Rando = MathUtils::RollNdM(DieCount, DieSides);
        UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx; Power (%f) * Magic (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"], BaseDamage, Rando);
        BaseDamage += Rando;
        FAttackTxInfo AttackTx;
        AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
        AttackTx.IsCrit = BaseDamage / (DieCount * DieSides) >= 0.9f;
        return AttackTx;
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("CalculateDamageTx; attacking AActor %s is not a BattleStatsBearer, so we cannot calc damage"), *BattleStatsBearer->GetName());
        return FAttackTxInfo();
    }
}