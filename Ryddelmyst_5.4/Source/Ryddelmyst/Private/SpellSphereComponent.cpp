// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellSphereComponent.h"
#include "MagicWeapon.h"

USpellSphereComponent::USpellSphereComponent()
{
    SetNotifyRigidBodyCollision(true);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName("BlockAllDynamic");
    Magic = CreateDefaultSubobject<UMagicWeapon>(TEXT("Magic Weapon"));
}

FBattleStatsData USpellSphereComponent::GetBattleStats_Implementation()
{ 
    return Magic->WielderData; 
}

UWeapon* USpellSphereComponent::GetWeapon_Implementation() 
{ 
    return Magic; 
}

FString USpellSphereComponent::GetCurrentAttack_Implementation() 
{ 
    return Magic->CurrentAttackName; 
}

void USpellSphereComponent::ExecuteAttack_Implementation
(
    const FString& AttackName
)
{ 
    // todo: kick off the attack's animation and any other immediate effects; an actual collision event later will lead to the attack's OnHit call. 
    //  EDIT: for now I'm using a more top-down approach to kicking off the attack's animation since it would be clunky to try to have this leaf node spellsphere guy trying to reach up to whatever skeletal mesh he might be associated with and change its animation graph state, if any.
}