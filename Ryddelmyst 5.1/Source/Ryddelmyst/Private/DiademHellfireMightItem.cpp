// Fill out your copyright notice in the Description page of Project Settings.


#include "DiademHellfireMightItem.h"
#include "RyddelmystCharacter.h"
#include "FireSnowball.h"
#include "SnowballAttack.h"
#include <vector>

const std::string UDiademHellfireMightItem::ID = "DiademHellFireMight";

void UDiademHellfireMightItem::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AFireSnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*UDiademHellfireMightItem::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_EVOCATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_ATTR] = [](ASnowball* Snowball)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; attr modifier effect"));
			UAttack* Attack = Snowball->GetSpellSphereComponent()->GetWeapon()->GetCurrentAttack();
			Cast<USnowballAttack>(Attack)->SetDamageScaleFactor(2.f);
		};
				
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AFireSnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*UDiademHellfireMightItem::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_ENCHANTMENT][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_EFFECT] = [](AActor* TargetActor, const FHitResult& HitResult)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; onhit effect"));
			if (TargetActor->IsRootComponentMovable())
			{
				FVector PreviousActorLocation = TargetActor->GetActorLocation();
				TargetActor->SetActorLocation(TargetActor->GetActorLocation() + HitResult.ImpactNormal * -250.f);
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; just moved %s from %s to %s via impact normal %s times -250"), 
					*TargetActor->GetName(), *PreviousActorLocation.ToString(), *TargetActor->GetActorLocation().ToString(), *HitResult.ImpactNormal.ToString()); 
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; can't apply transform onhit effect to stationary target actor %s"), *TargetActor->GetName());
			}
		};
	}
		
}

