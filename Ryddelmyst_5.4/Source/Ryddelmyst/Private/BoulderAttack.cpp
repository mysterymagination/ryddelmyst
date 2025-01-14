// Fill out your copyright notice in the Description page of Project Settings.


#include "BoulderAttack.h"
#include "BludgeoningDamageType.h"
#include "MathUtils.h"

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
    // impart impulse force to the stricken actor proportionate to the boulder mass iff stricken actor is a character subclass
    if (ACharacter* Character = Cast<ACharacter>(StrickenActor))
    {
        NormalImpulse.Normalize(0);
        UE_LOG(LogTemp, Warning, TEXT("BoulderAttack OnHit; inflicting custom knockback via mass piece %f times normalized impluse vector %s"), Mass*500.f, *NormalImpulse.ToString());
        Character->LaunchCharacter((Mass*500.f) * NormalImpulse, false, false);
        // todo: the full physkiss impulse would be nice, but that requires having physics enabled stricken components, which modifies the collision profiles we can use etc. Doable, but painful.
        // StrickenComp->AddImpulseAtLocation(Mass * NormalImpulse, StrickenActor->GetActorLocation());
    }
}

FAttackTxInfo UBoulderAttack::CalculateDamageTx_Implementation(FBattleStatsData BattleStatsData)
{
    float BaseDamage = BasePower * BattleStatsData.StatsMap["Attack"] * (Mass/10.f);
    uint8 DieCount = BattleStatsData.StatsMap["Level"];
    float Rando = MathUtils::RollNdM(DieCount, DieSides);
    UE_LOG(LogTemp, Warning, TEXT("CalculateDamageTx boulder atk; Power (%f) * Attack (%f) * Mass over ten (%f) = BaseDamage (%f) and rando aspect is %f"), BasePower, BattleStatsData.StatsMap["Attack"], Mass/10.f, BaseDamage, Rando);
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