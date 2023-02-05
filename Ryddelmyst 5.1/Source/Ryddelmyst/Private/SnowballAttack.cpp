// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowballAttack.h"

USnowballAttack::USnowballAttack()
{
    Costs = 
    {
        {
            "MP", 10.f
        }
    }
}

void USnowballAttack::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; HitComp says %s, OtherActor says %s, OtherComp says %s, normal impulse says %s, hitres says %s"), 
		*HitComp->GetName(), *OtherActor->GetName(), *OtherComp->GetName(), *NormalImpulse.ToString(), *Hit.ToString());
	UE_LOG(LogTemp, Warning, TEXT("OnSnowballHit; EffectsOnTarget has size %d"), EffectsOnTarget.size());
    Super::OnHit_Implementation(StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // auxiliary effect lambdas that Snowball spell can apply
	for (auto Effect : EffectsOnTarget)
	{
		Effect(OtherActor, Hit);
	}
    // todo: would be cleaner to have this logic in the Snowball Actor itself; maybe by registering a separate OnHit or similar function delegate with the 
    //  same OnComponentHit event that got us here?  Is there any guarantee of the order in which delegate functions are called by a triggering event, I wonder?
    // after expending its Attack, the Snowball Actor hierarchy is destroyed
    StrikingComp->GetOwnerActor()->Destroy();
}

float USnowballAttack::CalculateDamageTx_Implementation(AActor* BattleStatsBearer)
{
    float BaseDamage = Power * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"];
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; Power (%f) * Magic (%f) = BaseDamage (%f)"), Power, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Magic"], BaseDamage);
	BaseDamage += MathUtils::RollNdM(IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Level"], 6);
	return DamageScaleFactor * BaseDamage;
}