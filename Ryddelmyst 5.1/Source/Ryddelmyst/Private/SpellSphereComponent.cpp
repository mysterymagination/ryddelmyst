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
    //Claw->AttackMap[AttackName]->...
}