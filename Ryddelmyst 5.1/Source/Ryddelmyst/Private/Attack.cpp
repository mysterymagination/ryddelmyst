// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack.h"
#include "IAnatomy.h"
#include "Armor.h"
#include "IDefender.h"
#include "Kismet/GameplayStatics.h"

void UAttack::OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) 
{
    UE_LOG(LogTemp, Warning, TEXT("OnHit; attack is %s by striking comp %s against stricken comp %s of stricken actor %s"), 
        *AttackName, *StrikingComp->GetName(), *StrickenComp->GetName(), *StrickenActor->GetName());
    if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
    {
        // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
        float dmg = CalculateDamageTx(StrikingComp->GetOwner());
        UArmor* StrickenArmor = IDefender::Execute_GetArmor(StrickenComp);
		float DamageRx = 0.f;
        if(StrickenComp->GetClass()->ImplementsInterface(UAnatomy::StaticClass()))
		{
			UAnatomyUnit* AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(StrickenComp);
            DamageRx = StrickenArmor->CalculateDamageRx(StrickenActor, AnatomyUnit, dmg, DamageTypes);
			AnatomyUnit->Debilitate(StrickenActor);
		}
        else 
        {
            DamageRx = StrickenArmor->CalculateDamageRx(StrickenActor, nullptr, dmg, DamageTypes);
        }
        // todo: this approach only works if we assume all attacks from components of an Actor who is also a Pawn, e.g. a Monster's claw or Maya's mighty
        //  jump kicks.  We may wish to eschew the UGameplayStatics::ApplyPointDamage() event so we can both have attacks that are not connected physically
        //  to a Pawn and also so that we can support multiple damage types.  C'mon Unreal APIs, one damage type?  Really?
        APawn* InstigatorPawn = Cast<APawn>(StrikingComp->GetOwner());
        if(InstigatorPawn)
        {
            UGameplayStatics::ApplyPointDamage(StrickenActor, DamageRx, NormalImpulse, HitInfo, InstigatorPawn->GetController(), StrikingComp->GetOwner(), DamageTypes[0]);
        }
        else 
        {
            UE_LOG(LogTemp, Error, TEXT("OnHit; failed to apply point damage because the striking component's owner %s is not a APawn"), *StrikingComp->GetOwner()->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
    }
}
