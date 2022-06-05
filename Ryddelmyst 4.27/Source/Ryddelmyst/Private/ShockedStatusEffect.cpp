// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockedStatusEffect.h"
#include "BattleStatsBearer.h"
#include "BattleStats.h"

UShockedStatusEffect::UShockedStatusEffect()
{
    Id = "ShockedStatusEffect";
}

void UShockedStatusEffect::OnEffectApplied(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectApplied; shock duration is %f and the time is %s.  We are shocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
    // check if actor is an IBattleStatsBearer
    if(EffectedActor->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
    {
        // apply stat scaling
        IBattleStatsBearer::Execute_GetStats(EffectedActor)->ScaleStats(StatScaleFactor);
        // todo: create a lightning particle effect around the effected actor
        // start a clock that after ShockDuration seconds re-enables Actor tick
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
        EffectedActor->GetWorldTimerManager().SetTimer(ShockTimerHandle, TimerDelegate, ShockDuration, false);
    }
}

void UShockedStatusEffect::OnEffectRemoved(AActor* EffectedActor)
{
    UE_LOG(LogTemp, Warning, TEXT("OnEffectRemoved; shock duration is %f and the time is %s.  We are unshocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
    // restore stats to normal from cached values
    IBattleStatsBearer::Execute_GetStats(EffectedActor)->ScaleStats(1.f/StatScaleFactor);
    // todo: remove lightning particle effect around the effected actor
}


