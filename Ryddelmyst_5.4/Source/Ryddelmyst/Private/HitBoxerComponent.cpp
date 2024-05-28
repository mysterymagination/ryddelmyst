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
		// sometimes StrickenActor is nullptr apparently?  At least while having giant debug custom uprims with hitboxers. BrushComponent at least seems to show up as nullptr StrickenActor.
		if(Attack_Check && StrickenActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; %s's striking comp %s weapon %s is running attack %s on %s's shapely %s"),
				*StrikingComp->GetOwner()->GetName(), *StrikingComp->GetName(), (Weapon ? *Weapon->GetName() : TEXT("null weapon")), *(*Attack_Check)->AttackName, *StrickenActor->GetName(), *StrickenComp->GetName());
			(*Attack_Check)->OnHit_Implementation(IAttacker::Execute_GetBattleStats(StrikingComp), StrikingComp, StrickenActor, StrickenComp, NormalImpulse, HitInfo);

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HitBoxer::OnHit; Striking comp %s wielding weapon %s either failed to find an attack (attack ptr is %p) or the stricken Actor came up nullptr (stricken actor is %p)"), *StrikingComp->GetName(), *Weapon->GetName(), Attack_Check, StrickenActor);
		}
	}
	else
	{
		if (StrikingComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s is not an IAttacker so can't proceed with attack"), *StrikingComp->GetName());
			if (StrickenActor && StrickenComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("HitBoxer::OnHit; Striking comp %s is not an IAttacker so can't proceed with attackprocessing against %s's lovely %s"),
					*StrikingComp->GetName(),
					*StrickenActor->GetName(),
					*StrickenComp->GetName());
			}
		}
	}
}
