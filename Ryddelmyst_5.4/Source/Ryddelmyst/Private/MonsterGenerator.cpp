// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterGenerator.h"

void UMonsterGenerator::BeginPlay()
{
    Super::BeginPlay();
    if (AutomaticSpawn)
    {
        GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UMonsterGenerator::AutoSpawnMonster, SpawnPeriod, true);
    }
}

void UMonsterGenerator::SpawnMonster(TSubclassOf<AMonster> MonsterType)
{
    if (MonsterType)
    {
        // todo: spawn the given monster.
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster; no monster type specified, ignoring."));
    }
}

void UMonsterGenerator::AutoSpawnMonster()
{
    // todo: randomly pick a monster from the SpawnableMonsterClasses; if it's empty do nothing and log error.
}