// Fill out your copyright notice in the Description page of Project Settings.


#include "LegCapsuleComponent.h"

void ULegCapsuleComponent::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
	if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
	{
		// Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
		UE_LOG(LogTemp, Warning, TEXT("OnHit; using attack name %s"), *LegUnit->GetCurrentAttack());
		float dmg = IAttacker::Execute_CalculateDamageTx(this, LegUnit->GetCurrentAttack(), StrikingComp->GetOwner());
		IDefender::Execute_CalculateDamageRx(StrickenComp, dmg, IAttacker::Execute_GetDamageTypes(this, LegUnit->GetCurrentAttack()));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
	}
}



