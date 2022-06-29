// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "MonsterAI.h"

AMonsterAI::AMonsterAI()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAI::BeginPlay()
{
    /* todo: let's just keep things simple for the BP approach until that works, then we can monkey with customization
    GetWorldTimerManager().SetTimer(TrackingTimerHandle, this, &AMonsterAI::TrackPlayer, TrackingPeriod, true);
    */
   Super::BeginPlay();
}

void AMonsterAI::TrackPlayer()
{
    UE_LOG(LogTemp, Warning, TEXT("TrackPlayer; refreshing player track behavior"));
    // pursue player 1
    /* todo: not working for some reason; try the BP node approach and see if that works
    MoveToActor(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0),
        100.f,
        true,
        true,
        false,
        nullptr,
        true
    );
    */
}
