// Fill out your copyright notice in the Description page of Project Settings.


#include "ClawCapsuleComponent.h"

UClawCapsuleComponent::UClawCapsuleComponent()
{
    Claw = CreateDefaultSubobject<UClawWeapon>(TEXT("Claw Weapon"));
}

UWeapon* UClawCapsuleComponent::GetWeapon_Implementation() 
{ 
    return Claw; 
}

void UClawCapsuleComponent::ExecuteAttack_Implementation
(
    UPrimitiveComponent* StrikingComp,
    AActor* StrickenActor,
    UPrimitiveComponent* StrickenComp,
    FVector NormalImpulse,
    const FHitResult& HitInfoconst,
    const FString& AttackName
)
{ 
    Claw->AttackMap[AttackName]->OnHit
    (
        StrikingComp,
        StrickenActor,
        StrickenComp,
        NormalImpulse,
        HitInfoconst
    ); 
}
