// Fill out your copyright notice in the Description page of Project Settings.

#include "ClawSlashAttack.h"
#include "SlashingDamageType.h"
#include "Armor.h"
#include "IDefender.h"
#include "MathUtils.h"

const FString UClawSlashAttack::ATTACK_NAME("Claw Slash Attack");

UClawSlashAttack::UClawSlashAttack()
{
    AttackName = ATTACK_NAME;
    BasePower = 10.f;
    DamageTypes = {USlashingDamageType::StaticClass()};
}

void UClawSlashAttack::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) 
{
    Super::OnHit_Implementation(StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);
    if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
    {
        // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
        UE_LOG(LogTemp, Warning, TEXT("OnHit; attack is %s"), *AttackName);
        float dmg = CalculateDamageTx(StrikingComp->GetOwner());
        UArmor* StrickenArmor = IDefender::Execute_GetArmor(StrickenComp);
		StrickenArmor->CalculateDamageRx(StrickenActor, dmg, DamageTypes);
		if(StrickenComp->GetClass()->ImplementsInterface(UAnatomy::StaticClass()))
		{
			UAnatomyUnit* AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(StrickenComp);
			AnatomyUnit->Debilitate(StrickenActor);
		}
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
    }
}

float UClawSlashAttack::CalculateDamageTx_Implementation(AActor* BattleStatsBearer)
{
    float BaseDamage = BasePower * IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Attack"];
	UE_LOG(LogTemp, Warning, TEXT("CalculateDamage; BasePower (%f) * Attack (%f) = BaseDamage (%f)"), BasePower, IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Attack"], BaseDamage);
	BaseDamage += MathUtils::RollNdM(IBattleStatsBearer::Execute_GetStats(BattleStatsBearer)->StatsMap["Level"], 8);
	return DamageScaleFactor * BaseDamage;
}
	