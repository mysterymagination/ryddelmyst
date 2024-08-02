// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterGenerator.h"

void MonsterGenerator::BeginPlay()
{
    Super::BeginPlay();
    if (AutomaticSpawn)
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AMonsterGenerator::SpawnMonster, SpawnPeriod, true);
    }
}

void MonsterGenerator::SpawnMonster(TSubclassOf<AMontser> MonsterType)
{
    if (MonsterType)
    {
        // todo: spawn the given monster.
    }
    else 
    {
        // todo: randomly pick a monster from the SpawnableMonsterClasses; if it's empty do nothing and log error.
    }
}