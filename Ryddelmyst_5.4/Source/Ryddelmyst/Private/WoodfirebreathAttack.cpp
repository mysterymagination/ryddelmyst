// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodfirebreathAttack.h"
#include "FireDamageType.h"
#include "LightningDamageType.h"
#include "StatusEffected.h"
#include "BurnedStatusEffect.h"
#include "MathUtils.h"

const FString UWoodfirebreathAttack::ATTACK_NAME("WoodfirebreathAttack");

UWoodfirebreathAttack::UWoodfirebreathAttack()
{
    DamageTypesToWeightsMap =
    {
        {UFireDamageType::StaticClass(), 0.5f},
        {ULightningDamageType::StaticClass(), 0.5f}
    };
    AttackName = ATTACK_NAME;
    BasePower = 1.f;
    Tags.Add(UAttack::TAG_FLAG_IGNORE_DEFENSE);
}

FAttackTxInfo UWoodfirebreathAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    // todo
}
void UWoodfirebreathAttack::OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    // todo
}