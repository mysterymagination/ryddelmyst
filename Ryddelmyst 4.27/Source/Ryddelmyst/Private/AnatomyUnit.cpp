// Fill out your copyright notice in the Description page of Project Settings.


#include "AnatomyUnit.h"

// todo: should we move the IAttacker and IDefender interfaces to the actual colliding components?  Maybe also add a function to IAttacker that black-boxes the hit processing, e.g. OnHit() that the collision event can call directly without needing any more information than what exists in the OnComponentHit() event API.  Snowball can just be an exception for now that does her own thing.
void UAnatomyUnit::OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
	if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
	{
		// Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
		UE_LOG(LogTemp, Warning, TEXT("OnHit; using attack name %s"), CurrentAttack);
		float dmg = IAttacker::Execute_CalculateDamageTx(this, CurrentAttack, StrikingComp->GetOwner());
		IDefender::Execute_CalculateDamageRx(StrickenComp, dmg, IAttacker::Execute_GetDamageTypes(this, CurrentAttack));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
	}
}


