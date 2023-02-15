// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBoxerComponent.h"
#include "IAttacker.h"
#include "Weapon.h"
#include "Attack.h"

void UHitBoxerComponent::OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s is part of AActor %s"), *StrikingComp->GetName(), *StrikingComp->GetOwner()->GetName());
	if (StrikingComp->GetClass()->ImplementsInterface(UAttacker::StaticClass()))
	{
		UWeapon* Weapon = IAttacker::Execute_GetWeapon(StrikingComp);
		UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s is wielding weapon %s"), *StrikingComp->GetName(), *Weapon->GetName());
		UAttack** Attack_Check = Weapon->AttackMap.Find(Weapon->CurrentAttackName);
		if(Attack_Check)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s weapon %s is running attack %s on %s's shapely %s"), 
				*StrikingComp->GetName(), *Weapon->GetName(), *(*Attack_Check)->AttackName, *StrickenActor->GetName(), *StrickenComp->GetName());
			(*Attack_Check)->OnHit_Implementation(IAttacker::Execute_GetBattler(StrikingComp), StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s is not an IAttacker so can't proceed with attack processing against %s's lovely %s"), 
			*StrikingComp->GetName(),
			*StrickenActor->GetName(),
			*StrickenComp->GetName());
	}
}

