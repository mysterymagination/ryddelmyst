// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtremityCapsuleComponent.h"

void UExtremityCapsuleComponent::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
    // todo: change this check to StrickenComp->GetArmor() and if non-null then go ahead and call the IDefender stuff on the Armor member; that way we can consolidate default behaviors for IAttacker and IDefender impls
    if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
    {
        // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
        UE_LOG(LogTemp, Warning, TEXT("OnHit; using attack name %s"), *ExUnit->GetCurrentAttack());
        float dmg = IAttacker::Execute_CalculateDamageTx(this, ExUnit->GetCurrentAttack(), StrikingComp->GetOwner());
        IDefender::Execute_CalculateDamageRx(StrickenComp, dmg, IAttacker::Execute_GetDamageTypes(this, ExUnit->GetCurrentAttack()));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
    }
}
