// Fill out your copyright notice in the Description page of Project Settings.


#include "WoodfirebreathAttack.h"
#include "FireDamageType.h"
#include "LightningDamageType.h"
#include "StatusEffected.h"
#include "BurnedStatusEffect.h"
#include "ShockedStatusEffect.h"
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
    float BaseDamage = BasePower * (BattleStatsData.StatsMap[PrimaryStat]/10.f);
    BaseDamage += (BasePower/2.f) * BattleStatsData.StatsMap[SecondaryStat];
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT(
            "CalculateDamageTx woodfirebreath atk; Power (%f) * primary stat %s (%f) + Power/2 (%f) * secondary stat %s (%f) = BaseDamage (%f) and rando aspect is %f"
        ), 
        BasePower, 
        *PrimaryStat, 
        BattleStatsData.StatsMap[PrimaryStat], 
        BasePower/2.f, 
        *SecondaryStat, 
        BattleStatsData.StatsMap[SecondaryStat], 
        BaseDamage, 
        Rando
    );
    BaseDamage += Rando;
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = Rando / static_cast<float>((DieCount * DieSides)) >= 0.5f;
    // add on a full diceroll of damage for crit
    if (AttackTx.IsCrit)
    {
        AttackTx.DamageTx += DieCount * DieSides;
    }
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx woodfirebreath atk; IsCrit is %d based on rando %f div max rando %f which is %f"), AttackTx.IsCrit, Rando, static_cast<float>(DieCount * DieSides), Rando / static_cast<float>((DieCount * DieSides)));
    return AttackTx;
}
void UWoodfirebreathAttack::OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    // The ignore def tag will cause the boilerplate onhit logic to skip target defense damage reduction
    Super::OnHit_Implementation(StrikingBattlerData, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    if(StrickenActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // We don't want to stack the same status effect type
        auto StatusEffectArray = IStatusEffected::Execute_GetStatusEffects(StrickenActor);
        if(StrickenActor->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
        {
            auto is_shocked = [](UStatusEffect* Effect) { return Effect->GetId() == UShockedStatusEffect::NAME; };
            UStatusEffect** ShockedStatusEffect_ptr = StatusEffectArray.FindByPredicate(is_shocked);
            if (!ShockedStatusEffect_ptr)
            {
                // todo: does making OtherActor the outer/owner and then also adding the status effect to a uprop'd tarray inside OtherActor cause the StatusEffect to be doubly referenced by OtherActor?  I want the StatusEffect to be owned by the target TArray, which itself is owned by OtherActor; StatusEffects removed from that array should be garbage collected.
                UShockedStatusEffect* ShockStatusEffect = NewObject<UShockedStatusEffect>(StrickenActor, UShockedStatusEffect::StaticClass());
                ShockStatusEffect->SetId("ShockedStatusEffect");
                ShockStatusEffect->SetShockDuration(ShockDuration);
                IStatusEffected::Execute_AddStatusEffect(StrickenActor, ShockStatusEffect);
                ShockStatusEffect->OnEffectApplied(StrickenActor);
            }
        }
        auto is_burned = [](UStatusEffect* Effect) { return Effect->GetId() == UBurnedStatusEffect::NAME; };
        UStatusEffect** BurnedStatusEffect_ptr = StatusEffectArray.FindByPredicate(is_burned);
        if (!BurnedStatusEffect_ptr)
        {
            UBurnedStatusEffect* BurnStatusEffect = NewObject<UBurnedStatusEffect>(StrickenActor, UBurnedStatusEffect::StaticClass());
            BurnStatusEffect->SetBaseBurnDuration(BurnDuration);
            BurnStatusEffect->SetBurnDuration(BurnDuration);
            BurnStatusEffect->SetBurnPeriod(BurnPeriod);
            // ongoing damage will be init calculated damage over 4
            BurnStatusEffect->SetBurnDamage(LatestDamageDealt / 4.f);
            IStatusEffected::Execute_AddStatusEffect(StrickenActor, BurnStatusEffect);
            BurnStatusEffect->OnEffectApplied(StrickenActor);
        }
    }
}