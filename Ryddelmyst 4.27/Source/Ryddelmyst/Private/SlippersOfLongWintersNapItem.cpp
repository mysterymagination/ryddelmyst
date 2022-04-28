// Fill out your copyright notice in the Description page of Project Settings.


#include "SlippersOfLongWintersNapItem.h"
#include "RyddelmystCharacter.h"
#include "Snowball.h"

const std::string USlippersOfLongWintersNapItem::ID = "SlippersLongWinterNap";

void USlippersOfLongWintersNapItem::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		Character->GetMetamagicMap()[ARyddelmystCharacter::ID_SPELL_SNOWBALL][USlippersOfLongWintersNapItem::ID][ARyddelmystCharacter::ID_SPELL_ASPECT_EVOCATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_ATTR] = [](ASnowball* Snowball)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Ice lambda; attr modifier effect"));
			Snowball->SetFreezeDuration(2.f);
		};
	}
}
