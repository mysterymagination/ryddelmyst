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
				FireSnowball->GetEffectsVector().emplace_back([](AActor* TargetActor)
					{
						UE_LOG(LogTemp, Warning, TEXT("Metamagic Fire lambda; onhit effect"));
						TargetActor->SetActorRelativeLocation(FVector(-50.f, 0.f, 0.f));
					}
				);
			}
		);
	}
}

