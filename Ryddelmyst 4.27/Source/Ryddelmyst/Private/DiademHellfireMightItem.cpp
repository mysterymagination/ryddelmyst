// Fill out your copyright notice in the Description page of Project Settings.


#include "DiademHellfireMightItem.h"
#include "RyddelmystCharacter.h"
#include "FireSnowball.h"
#include <vector>

void UDiademHellfireMightItem::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		Character->SetMetaMagicFire([](AFireSnowball* FireSnowball) 
			{
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; modifier effect"));
				FireSnowball->SetDamageScaleFactor(2.f);
				FireSnowball->GetEffectsVector().emplace_back([](AActor* TargetActor, const FHitResult& HitResult)
					{
						UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; onhit effect"));
						if (TargetActor->IsRootComponentMovable())
						{
							TargetActor->SetActorLocation(TargetActor->GetActorLocation() + HitResult.ImpactNormal * -250.f);
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; can't apply transform onhit effect to stationary target actor %s"), *TargetActor->GetName());
						}
					}
				);
			}
		);
	}
}

