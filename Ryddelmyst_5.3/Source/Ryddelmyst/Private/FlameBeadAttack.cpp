// Fill out your copyright notice in the Description page of Project Settings.

#include "FlameBeadAttack.h"
#include "FireDamageType.h"
#include "StatusEffected.h"
#include "BurnedStatusEffect.h"
#include "MathUtils.h"

const FString UFlameBeadAttack::ATTACK_NAME("FlameBeadAttack");

UFlameBeadAttack::UFlameBeadAttack()
{
    DamageTypesToWeightsMap =
    {
        {UFireDamageType::StaticClass(), 1.f}
    };
    AttackName = ATTACK_NAME;
    BasePower = 1.f;
}

void UFlameBeadAttack::OnHit_Implementation(FBattleStatsData StrikingBattlerData, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    Super::OnHit_Implementation(StrikingBattlerData, StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    // elemental StatusEffect
    if (StrickenActor->GetClass()->ImplementsInterface(UStatusEffected::StaticClass()))
    {
        // check if the target is already burned and extend the duration by BaseBurnDuration if so, up to DieSides * BaseBurnDuration
        auto StatusEffectArray = IStatusEffected::Execute_GetStatusEffects(StrickenActor);
        auto is_burn = [](UStatusEffect* Effect) { return Effect->GetId() == UBurnedStatusEffect::NAME; };
        if (auto it = std::find_if(std::begin(StatusEffectArray), std::end(StatusEffectArray), is_burn); it != std::end(StatusEffectArray))
        {
            float CurrentBurnDuration = Cast<UBurnedStatusEffect>(*it)->GetBurnDuration();
            float BaseBurnDuration = Cast<UBurnedStatusEffect>(*it)->GetBaseBurnDuration();
            if (CurrentBurnDuration < DieSides * BaseBurnDuration)
            {
                Cast<UBurnedStatusEffect>(*it)->SetBurnDuration(CurrentBurnDuration + BaseBurnDuration);
            }
        }
        else
        {
            UBurnedStatusEffect* StatusEffect = NewObject<UBurnedStatusEffect>(StrickenActor, UBurnedStatusEffect::StaticClass());

            StatusEffect->SetBaseBurnDuration(BurnDuration);
            StatusEffect->SetBurnDuration(BurnDuration);
            StatusEffect->SetBurnPeriod(BurnPeriod);
            // ongoing damage will be init calculated damage over 4
            StatusEffect->SetBurnDamage(LatestDamageDealt / 4.f);
            UE_LOG(LogTemp, Warning, TEXT("OnHit; applying primary StatusEffect with id %s"), *StatusEffect->GetId());
            IStatusEffected::Execute_AddStatusEffect(StrickenActor, StatusEffect);
            StatusEffect->OnEffectApplied(StrickenActor);
        }
    }
}

FAttackTxInfo UFlameBeadAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Magic"];
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx flame bead atk; Power (%f) * Magic (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, BattleStatsData.StatsMap["Magic"], BaseDamage, Rando);
    BaseDamage += Rando;
    FAttackTxInfo AttackTx;
    AttackTx.DamageTx = DamageScaleFactor * BaseDamage;
    AttackTx.IsCrit = Rando / static_cast<float>((DieCount * DieSides)) >= 0.5f;
    // add on a full diceroll of damage for crit
    if (AttackTx.IsCrit)
    {
        AttackTx.DamageTx += DieCount * DieSides;
    }
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx flame bead atk; IsCrit is %d based on rando %f div max rando %f which is %f"), AttackTx.IsCrit, Rando, static_cast<float>(DieCount * DieSides), Rando / static_cast<float>((DieCount * DieSides)));
    return AttackTx;
}
