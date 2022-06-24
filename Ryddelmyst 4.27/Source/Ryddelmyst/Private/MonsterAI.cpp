// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterAI.h"


void AMonsterAI::BeginPlay()
{
    GetWorldTimerManager().SetTimer(TrackingTimerHandle, this, &AMonsterAI::TrackPlayer, TrackingPeriod, true);
}

void AMonsterAI::TrackPlayer()
{
    // todo: ape AI Move To node
}
