// Fill out your copyright notice in the Description page of Project Settings.


#include "Attack.h"
#include "IAnatomy.h"
#include "Armor.h"
#include "IDefender.h"
#include "RyddelmystCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DataViz.h"

const FString UAttack::KEY_COSTS_EFFECT("Effect");

void UAttack::OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) 
{
    UE_LOG(LogTemp, Warning, TEXT("OnHit; attack is %s by striking comp %s against stricken comp %s of stricken actor %s"), 
        *AttackName, *StrikingComp->GetName(), *StrickenComp->GetName(), *StrickenActor->GetName());
    if(StrikingBattler)
    {
        UArmor* StrickenArmor = nullptr;
        UAnatomyUnit* AnatomyUnit = nullptr;
        AActor* StrickenBattler = nullptr;
        // todo: it might be better to have the attack impl simply figure out how much the damagetx is and then call ApplyPointDamage or a similar event that the
        //  relevant defender handles and then applies its Armor DR etc. on its end; that would separate the responsibilities of the code a little better and
        //  make Attack and Armor less tightly coupled.
        if (StrickenComp->GetClass()->ImplementsInterface(UDefender::StaticClass()))
        {
            StrickenArmor = IDefender::Execute_GetArmor(StrickenComp);
            StrickenBattler = IDefender::Execute_GetBattler(StrickenComp);
            if(StrickenComp->GetClass()->ImplementsInterface(UAnatomy::StaticClass()))
            {
                AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(StrickenComp);
            }
        }
        /*
        else 
        {
            // todo: remove this branch hack if the query collision events on custom uprim IDefenders and IAttackers ever works properly
            if(StrickenActor->GetClass()->IsChildOf(ARyddelmystCharacter::StaticClass()))
            {
                // todo: worst case scenario, if the query collision never works right we can try to figure out where the collision happened in Maya's local space and guess at the hitbox it should have collided with.
                ARyddelmystCharacter* Ryddelmystress = Cast<ARyddelmystCharacter>(StrickenActor);
                UBodyCapsuleComponent* Body = Ryddelmystress->GetBody();
                if(Body)
                {
                    StrickenArmor = IDefender::Execute_GetArmor(Body);
                    StrickenBattler = IDefender::Execute_GetBattler(Body);
                    AnatomyUnit = IAnatomy::Execute_GetAnatomyUnit(Body);
                    UE_LOG(LogTemp, Log, TEXT("OnHit; looked up the stricken stuff using hardcoded RMystCharacter cast hack. Armor says: %s, StrickenBattler says: %s, and Anatomy says: %s"),
                        *StrickenArmor->GetName(), *StrickenBattler->GetName(), *AnatomyUnit->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("OnHit; failed to find Maya's body in the ryddelmystress hack branch to work around the stupid custom uprim collision issue"));
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("OnHit; the stricken component %s does not implement IDefender, so we can't move forward communicating damage"), *StrickenComp->GetName());
            }
        }
        */
        if(StrickenArmor)
        {
            // Damage setter is inside the IDefender target check so that we only bother calc/cache of damage if we can actually apply the damage
            FAttackTxInfo DamageTxInfo = CalculateDamageTx(StrikingBattler);
            float DamageRx = 0.f;
            if(AnatomyUnit)
            {
                DamageRx = StrickenArmor->CalculateDamageRx(StrickenBattler, AnatomyUnit, DamageTxInfo.DamageTx, DamageTypesToWeightsMap);
                AnatomyUnit->Debilitate(StrickenActor);
            }
            else 
            {
                DamageRx = StrickenArmor->CalculateDamageRx(StrickenBattler, nullptr, DamageTxInfo.DamageTx, DamageTypesToWeightsMap);
            }
            LatestDamageDealt = DamageRx;
            // todo: this approach only works if we assume all attacks come from components of an Actor who is also a Pawn, e.g. a Monster's claw or Maya's mighty
            //  jump kicks.  We may wish to eschew the UGameplayStatics::ApplyPointDamage() event so we can both have attacks that are not connected physically
            //  to a Pawn and also so that we can support multiple damage types.  C'mon Unreal APIs, one damage type?  Really?
            
            APawn* InstigatorPawn = Cast<APawn>(StrikingBattler);
            if(InstigatorPawn)
            {
                TArray<TSubclassOf<UDamageType>> Types;
                DamageTypesToWeightsMap.GenerateKeyArray(Types);

                UE_LOG(LogTemp, Log, TEXT("OnHit; FHitResult says: %s, specifically BoneName is %s and MyBoneName is %s"), *HitInfo.ToString(), *HitInfo.BoneName.ToString(), *HitInfo.MyBoneName.ToString()); 
                UE_LOG(LogTemp, Log, TEXT("OnHit; striking component says: %s"), *StrikingComp->GetName());
                UE_LOG(LogTemp, Log, TEXT("OnHit; stricken component says: %s"), *StrickenComp->GetName()); 
                if (StrickenActor->CanBeDamaged())
                {
                    UGameplayStatics::ApplyPointDamage(StrickenBattler, DamageRx, NormalImpulse, HitInfo, InstigatorPawn->GetController(), StrikingBattler, Types[0]);
                    DataViz::FX_NumberParticles
                    (
                        GetWorld(),
                        HitInfo.Location,
                        FRotator(0.f),
                        FVector(1.f),
                        DamageRx,
                        DamageRx / DamageTxInfo.DamageTx,
                        DamageTxInfo.IsCrit
                    );
                    UE_LOG(LogTemp, Warning, TEXT("OnHit; applied point damage of %f to %s"), DamageRx, *StrickenBattler->GetName());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("OnHit; skipped applying point damage of %f to %s because of iframes"), DamageRx, *StrickenBattler->GetName());
                }
            }
            else 
            {
                UE_LOG(LogTemp, Error, TEXT("OnHit; failed to apply point damage of %f because the striking component's owner %s is not a APawn"), DamageRx, *StrikingBattler->GetName());
            }
            
            /*
            TArray<TSubclassOf<UDamageType>> Types;
            DamageTypesToWeightsMap.GenerateKeyArray(Types);
            AController* MonsterInstigator = Cast<ACharacter>(StrikingBattler)->GetController();
            if (MonsterInstigator) 
            {
                UGameplayStatics::ApplyPointDamage(StrickenBattler, DamageRx, NormalImpulse, HitInfo, MonsterInstigator, StrikingBattler, Types[0]);
                UE_LOG(LogTemp, Warning, TEXT("OnHit; applied point damage of %f to %s"), DamageRx, *StrickenBattler->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("OnHit; failed to apply point damage of %f to %s because we failed to look up the instigator controller of %s"), DamageRx, *StrickenBattler->GetName(), *StrikingBattler->GetName());
            }
            */
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("OnHit; failed to extract Armor from the stricken component %s or actor %s"), *StrickenComp->GetName(), *StrickenActor->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHit; the strikingbattler pointer %p is nullptr so we can't move forward communicating damage"), StrikingBattler);
    }
}
