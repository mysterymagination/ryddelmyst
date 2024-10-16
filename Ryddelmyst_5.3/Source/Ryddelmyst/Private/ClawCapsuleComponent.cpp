// Fill out your copyright notice in the Description page of Project Settings.


#include "ClawCapsuleComponent.h"

UClawCapsuleComponent::UClawCapsuleComponent()
{
    SetNotifyRigidBodyCollision(true);
	SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SetCollisionProfileName("BlockAllDynamic");
    Claw = CreateDefaultSubobject<UClawWeapon>(TEXT("Claw Weapon"));
}

FBattleStatsData UClawCapsuleComponent::GetBattleStats_Implementation()
{ 
    // simple case since we can assume the owning Actor would also be the Weapon's wielder
    FBattleStatsData Data = IBattleStatsBearer::Execute_GetStats(GetOwner())->StatsData;
    Data.BattlerTransform = GetOwner()->GetActorTransform();
    Claw->WielderData = Data;
    return Claw->WielderData;
}

UWeapon* UClawCapsuleComponent::GetWeapon_Implementation() 
{ 
    return Claw; 
}

FString UClawCapsuleComponent::GetCurrentAttack_Implementation() 
{ 
    return Claw->CurrentAttackName; 
}

void UClawCapsuleComponent::ExecuteAttack_Implementation
(
    const FString& AttackName
)
{ 
    // todo: kick off the attack's animation and any other immediate effects; an actual collision event later will lead to the attack's OnHit call. 
    //Claw->AttackMap[AttackName]->...
}
