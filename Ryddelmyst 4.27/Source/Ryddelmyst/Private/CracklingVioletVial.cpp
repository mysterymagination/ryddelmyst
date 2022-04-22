// Fill out your copyright notice in the Description page of Project Settings.


#include "CracklingVioletVial.h"
#include "RyddelmystCharacter.h"
#include "ElectricSnowball.h"

void UCracklingVioletVial::OnEquip_Implementation(AActor* EquippedActor)
{
	ARyddelmystCharacter* Character = Cast<ARyddelmystCharacter>(EquippedActor);
	if (Character)
	{
		// todo: we'll want to be able to apply projectile spawn behavior per element of snowball; TBD how best to achieve this
		Character->GetMetamagicMap()[ARyddelmystCharacter::ID_SPELL_ELECTRICSNOWBALL][UCracklingVioletVial::ID][ARyddelmystCharacter::ID_SPELL_ASPECT_CONJURATION][ARyddelmystCharacter::ID_METAMAGIC_CATEGORY_SPAWN] = [=](ARyddelmystCharacter* ConjuringCharacter, const FVector& SpawnLocation, const FVector& LaunchDirection)
		{
			UE_LOG(LogTemp, Warning, TEXT("Metamagic Electric lambda; conjuration behavior"));
			// todo: spawn N electric snowballs in a uniform distribution arc; assuming N is odd, take the actual given launch direction and use it as the center snowball (0 degrees rotation), then make (N-1)/2 vectors rotated ccw and the same number rotated cw.  The angle between each 'fanning' vector will be 90/((N-1)/2 + 1) degrees (the +1 to account for the fact that we don't want to actually reach a full 90 degrees so we're creating a sort of phantom extra vector for the distribution that doesn't actually get used), so for 5 snowballs we'd have 0 degrees, 30 degrees, 60 degrees, -30 degrees, and -60 degrees vectors.
		};
	}
}