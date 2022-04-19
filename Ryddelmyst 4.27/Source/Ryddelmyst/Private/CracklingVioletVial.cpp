// Fill out your copyright notice in the Description page of Project Settings.


#include "CracklingVioletVial.h"
#include "RyddelmystCharacter.h"
#include "ElectricSnowball.h"

void UCracklingVioletVial::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		// todo: we'll want to be able to apply launch behavior per element of snowball; TBD how best to achieve this
		Character->SetMetaMagicLightning([=](AElectricSnowball* ElectricSnowball)
			{
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Electric lambda; modifier effect"));
				Character->SetMetaMagicLaunchBehavior([=](ASnowball* Snowball)
					{
						// todo: spreadshot
					}
				);
			}
		);
	}
}