// Fill out your copyright notice in the Description page of Project Settings.


#include "ShockedStatusEffect.h"
#include "BattleStatsBearer.h"
#include "BattleStats.h"

const FString UShockedStatusEffect::NAME = "ShockedStatusEffect";

UShockedStatusEffect::UShockedStatusEffect()
{
    Id = "ShockedStatusEffect";
}

void UShockedStatusEffect::OnEffectApplied(AActor* EffectedActor)
{
    if (IsValid(EffectedActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("OnEffectApplied; shock duration is %f and the time is %s.  We are shocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
        // check if actor is an IBattleStatsBearer
        if (EffectedActor->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
        {
            UE_LOG(LogTemp, Warning, TEXT("ShockedStatusEffect::OnEffectApplied; apply shock to %s"), *EffectedActor->GetName());
            // apply stat scaling
            IBattleStatsBearer::Execute_GetStats(EffectedActor)->ScaleStats(StatScaleFactor, EffectedActor);
            // todo: create a lightning particle effect around the effected actor
            // start a clock that after ShockDuration seconds re-enables Actor tick
            FTimerDelegate TimerDelegate;
            TimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
            EffectedActor->GetWorldTimerManager().SetTimer(ShockTimerHandle, TimerDelegate, ShockDuration, false);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("ShockedStatusEffect::OnEffectApplied; cannot apply shock to %s because they are not a battlestatsbearer"), *EffectedActor->GetName());
        }
    }
}

void UShockedStatusEffect::OnEffectRemoved(AActor* EffectedActor)
{
    if (IsValid(EffectedActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("OnEffectRemoved; shock duration is %f and the time is %s.  We are unshocking the actor: %s from shock status effect %p"), ShockDuration, *FDateTime::Now().ToString(), *EffectedActor->GetName(), this);
        // restore stats to normal from cached values
        IBattleStatsBearer::Execute_GetStats(EffectedActor)->ScaleStats(1.f / StatScaleFactor, EffectedActor);
        // todo: remove lightning particle effect around the effected actor
    }
}


