// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockedStatusEffect.h"

void UFrozenStatusEffect::OnEffectApplied(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectApplied; shock duration is %f and the time is %s.  We are shocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
    // todo: check if actor has an IRPGStats
    // todo: cache unmodified stats
    // todo: apply stat scaling
    // todo: create a lightning particle effect around the effected actor

    // start a clock that after ShockDuration seconds re-enables Actor tick
    FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
	EffectedActor->GetWorldTimerManager().SetTimer(ShockTimerHandle, TimerDelegate, ShockDuration, false);

}

void UFrozenStatusEffect::OnEffectRemoved(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectRemoved; shock duration is %f and the time is %s.  We are unshocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
    // todo: restore stats to normal from cached values
    // todo: remove lightning particle effect around the effected actor

    
}


