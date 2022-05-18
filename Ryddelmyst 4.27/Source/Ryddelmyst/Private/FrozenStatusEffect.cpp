// Fill out your copyright notice in the Description page of Project Settings.


#include "FrozenStatusEffect.h"

void UFrozenStatusEffect::OnEffectApplied(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectApplied; freezing the actor: %s"), *EffectedActor->GetName());
    // disable Actor tick
    EffectedActor->SetActorTickEnabled(false);
    if(UMovementComponent* MovementComponent = EffectedActor->FindComponentByClass<UMovementComponent>())
    {
        MovementComponent->SetComponentTickEnabled(false);
    }

    // todo: create a snowflake particle effect around the effected actor

    // start a clock that after FreezeDuration seconds re-enables Actor tick
    FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
	EffectedActor->GetWorldTimerManager().SetTimer(FreezeTimerHandle, TimerDelegate, FreezeDuration, false);

}

void UFrozenStatusEffect::OnEffectRemoved(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectRemoved; unfreezing the actor: %s"), *EffectedActor->GetName());
    // todo: remove snowflake particle effect around the effected actor

    // re-enable Actor tick
    EffectedActor->SetActorTickEnabled(true);
    if(UMovementComponent* MovementComponent = EffectedActor->FindComponentByClass<UMovementComponent>())
    {
        MovementComponent->SetComponentTickEnabled(true);
    }
}


