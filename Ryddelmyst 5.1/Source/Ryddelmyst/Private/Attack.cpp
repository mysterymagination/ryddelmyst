// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack.h"
#include "IAnatomy.h"
#include "Armor.h"
#include "IDefender.h"
#include "Kismet/GameplayStatics.h"

const FString UAttack::KEY_COSTS_EFFECT("Effect");

void UAttack::OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) 
{
    UE_LOG(LogTemp, Warning, TEXT("OnHit; attack is %s by striking comp %s against stricken comp %s of stricken actor %s"), 
        *AttackName, *StrikingComp->GetName(), *StrickenComp->GetName(), *StrickenActor->GetName());
    if (StrikingBattler && StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
    {
        // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
        float dmg = CalculateDamageTx(StrikingBattler);
        UArmor* StrickenArmor = IDefender::Execute_GetArmor(StrickenComp);
		float DamageRx = 0.f;
        if(StrickenComp->GetClass()->ImplementsInterface(UAnatomy::StaticClass()))
		{
			UAnatomyUnit* AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(StrickenComp);
            DamageRx = StrickenArmor->CalculateDamageRx(IDefender::Execute_GetBattler(StrickenComp), AnatomyUnit, dmg, DamageTypesToWeightsMap);
			AnatomyUnit->Debilitate(StrickenActor);
		}
        else 
        {
            DamageRx = StrickenArmor->CalculateDamageRx(IDefender::Execute_GetBattler(StrickenComp), nullptr, dmg, DamageTypesToWeightsMap);
        }
        // todo: this approach only works if we assume all attacks from components of an Actor who is also a Pawn, e.g. a Monster's claw or Maya's mighty
        //  jump kicks.  We may wish to eschew the UGameplayStatics::ApplyPointDamage() event so we can both have attacks that are not connected physically
        //  to a Pawn and also so that we can support multiple damage types.  C'mon Unreal APIs, one damage type?  Really?
        // todo: it might be better to have the attack impl simply figure out how much the damagetx is and then call ApplyPointDamage or a similar event that the
        //  relevant defender handles and then applies its Armor DR etc. on its end; that would separate the responsibilities of the code a little better and
        //  make Attack and Armor less tightly coupled.
        APawn* InstigatorPawn = Cast<APawn>(StrikingBattler);
        if(InstigatorPawn)
        {
            TArray<TSubclassOf<UDamageType>> Types;
            DamageTypesToWeightsMap.GenerateKeyArray(Types);
            UGameplayStatics::ApplyPointDamage(IDefender::Execute_GetBattler(StrickenComp), DamageRx, NormalImpulse, HitInfo, InstigatorPawn->GetController(), StrikingBattler, Types[0]);
        }
        else 
        {
            UE_LOG(LogTemp, Error, TEXT("OnHit; failed to apply point damage because the striking component's owner %s is not a APawn"), *StrikingBattler->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; either the strikingbattler pointer %p is nullptr or the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), StrikingBattler, *StrickenComp->GetName());
    }
}
