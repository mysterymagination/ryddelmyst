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
    // todo: this impl is probably wrong; what we should be doing here is kicking off the animation and then letting an actual collision lead to the attack OnHit call.  However, we'd need a way to propagate the AttackName through the async process of starting an attack anim (and any other pre-collision effects) and then later the actual OnHit.
    Claw->AttackMap[AttackName]->OnHit
    (
        StrikingComp,
        StrickenActor,
        StrickenComp,
        NormalImpulse,
        HitInfoconst
    ); 
}
