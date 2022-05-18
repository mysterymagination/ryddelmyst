// Fill out your copyright notice in the Description page of Project Settings.


#include "BurnedStatusEffect.h"
#include "FireDamageType.h"
#include "Kismet/GameplayStatics.h"

void UBurnedStatusEffect::OnEffectApplied(AActor* EffectedActor)
{
    // start a timer that runs a function to apply fire damage at the given period
    FTimerDelegate BurnTimerDelegate;
	BurnTimerDelegate.BindUFunction(this, FName("Burn"), EffectedActor);
    EffectedActor->GetWorldTimerManager().SetTimer(BurnTimerHandle, BurnTimerDelegate, BurnPeriod, true);
    // start a timer that will stop the burn damage application timer when the burn duration expires
    FTimerDelegate BurnCancelTimerDelegate;
	BurnCancelTimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
    EffectedActor->GetWorldTimerManager().SetTimer(TotalBurnTimerHandle, BurnCancelTimerDelegate, BurnDuration, false);
}

void UBurnedStatusEffect::OnEffectRemoved(AActor* EffectedActor)
{
    // cancel the burn timer
    EffectedActor->GetWorldTimerManager().ClearTimer(BurnTimerHandle);
}

void UBurnedStatusEffect::Burn(AActor* EffectedActor)
{
    // todo: surround Actor with fire particle effect
    UGameplayStatics::ApplyDamage(EffectedActor, BurnDamage, nullptr, nullptr, UFireDamageType::StaticClass());
}


