// Fill out your copyright notice in the Description page of Project Settings.


#include "SlippersOfLongWintersNapItem.h"
#include "RyddelmystCharacter.h"
#include "IceySnowball.h"

const std::string USlippersOfLongWintersNapItem::ID = "SlippersLongWinterNap";

void USlippersOfLongWintersNapItem::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		Character->GetMetamagicMap()[std::string(TCHAR_TO_UTF8(*AIceySnowball::StaticClass()->GetName()))][std::string(TCHAR_TO_UTF8(*USlippersOfLongWintersNapItem::StaticClass()->GetName()))][ARyddelmystCharacter::ID_SPELL_PHASE_EVOCATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_ATTR] = [](ASnowball* Snowball)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Ice lambda; attr modifier effect"));
			// todo: how come std::variant didn't seem to want me storing a lambda sig of both ASnowball and AIceySnowball (and nothing else, such that they're identical other than the input type)?  I guess it figured since AIceySnowball is also ASnowball that it would be ambiguous... but it isn't and that's stupid
			Cast<AIceySnowball>(Snowball)->SetFreezeDuration(2.f);
		};
	}
}
