// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellStaticMeshComponent.h"

USpellStaticMeshComponent::USpellStaticMeshComponent()
{
	SetNotifyRigidBodyCollision(true);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetCollisionProfileName("BlockAllDynamic");
	Magic = CreateDefaultSubobject<UMagicWeapon>(TEXT("Magic Weapon"));
}

AActor* USpellStaticMeshComponent::GetBattleStats_Implementation()
{
    return Magic->Wielder;
}

UWeapon* USpellStaticMeshComponent::GetWeapon_Implementation()
{
    return Magic;
}

FString USpellStaticMeshComponent::GetCurrentAttack_Implementation()
{
    return Magic->CurrentAttackName;
}

void USpellStaticMeshComponent::ExecuteAttack_Implementation
(
    const FString& AttackName
)
{
    // todo: kick off the attack's animation and any other immediate effects; an actual collision event later will lead to the attack's OnHit call. 
    //  EDIT: for now I'm using a more top-down approach to kicking off the attack's animation since it would be clunky to try to have this leaf node spellsphere guy trying to reach up to whatever skeletal mesh he might be associated with and change its animation graph state, if any.
}
