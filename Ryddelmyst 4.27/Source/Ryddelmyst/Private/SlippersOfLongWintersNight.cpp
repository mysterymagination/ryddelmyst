// Fill out your copyright notice in the Description page of Project Settings.


#include "SlippersOfLongWintersNight.h"
#include "RyddelmystCharacter.h"
#include "Snowball.h"

void USlippersOfLongWintersNight::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		Character->SetMetaMagicIce([](ASnowball* Snowball)
			{
				UE_LOG(LogTemp, Warning, TEXT("Metamagic Ice lambda; modifier effect"));
				Snowball->SetFreezeDuration(Snowball->GetFreezeDuration() * 3.f);
			}
		);
	}
}
