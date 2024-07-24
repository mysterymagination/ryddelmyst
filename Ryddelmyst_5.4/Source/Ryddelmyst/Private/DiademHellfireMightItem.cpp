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
			UAttack* Attack = IAttacker::Execute_GetWeapon(Snowball->GetSpellSphereComponent())->GetCurrentAttack();
			Cast<USnowballAttack>(Attack)->SetDamageScaleFactor(2.f);
		};
				
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AFireSnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*UDiademHellfireMightItem::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_ENCHANTMENT][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_EFFECT] = [](AActor* TargetActor, const FHitResult& HitResult)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; onhit effect"));
			if (TargetActor->IsRootComponentMovable())
			{
				/* todo: for some reason turning physics on for the treant hitbox components causes snowballs not to register hits, and if I enable on the collision cyl he goes flying like a cartoon balloon unless I have it ignore HitBox collision type. That can be fine, but then he's rolling around all over the place after teleport for some reason.
				TArray<UPrimitiveComponent*> OutPrims;
				TargetActor->GetComponents<UPrimitiveComponent>(OutPrims, true);
				for (auto Prim : OutPrims)
				{
					// todo: first check if physics is enabled for comp
					Prim->AddImpulse(HitResult.ImpactNormal * 1500.f, NAME_None, true);
				}
				*/
				/* reliable but silly simple teleport back approach
				FVector PreviousActorLocation = TargetActor->GetActorLocation();
				TargetActor->SetActorLocation(TargetActor->GetActorLocation() + HitResult.ImpactNormal * -250.f);
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; just moved %s from %s to %s via impact normal %s times -250"), 
					*TargetActor->GetName(), *PreviousActorLocation.ToString(), *TargetActor->GetActorLocation().ToString(), *HitResult.ImpactNormal.ToString()); 
				*/
				if (ACharacter* Character = Cast<ACharacter>(TargetActor))
				{
					FVector NormalImpulseVector = HitResult.ImpactNormal;
					NormalImpulseVector.Normalize(0);
					Character->LaunchCharacter(5000.f * -NormalImpulseVector, false, false);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; can't apply transform onhit effect to stationary target actor %s"), *TargetActor->GetName());
			}
		};
	}
		
}

