// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "MonsterAI.h"


void AMonsterAI::BeginPlay()
{
    GetWorldTimerManager().SetTimer(TrackingTimerHandle, this, &AMonsterAI::TrackPlayer, TrackingPeriod, true);
}

void AMonsterAI::TrackPlayer()
{
    // todo: add navmesh to megyle
    UE_LOG(LogTemp, Warning, TEXT("TrackPlayer; refreshing player track behavior"));
    // pursue player 1
    MoveToActor(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0),
        100.f,
        true,
        true,
        false,
        nullptr,
        true
    );
}
