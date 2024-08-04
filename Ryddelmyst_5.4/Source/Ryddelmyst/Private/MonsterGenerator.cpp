// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterGenerator.h"
#include "Math/UnrealMathUtility.h"

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
        // spawn an instance of the given monster type at the location of the 
        // owning actor of the generator.
        FVector Location = GetOwner()->GetActorLocation();
        const FVector* Location_ptr = &Location;
        FRotator Rotation = GetOwner()->GetActorRotation();
        const FRotator* Rotation_ptr = &Rotation;
        const FActorSpawnParameters SpawnInfo;
        GetWorld()->SpawnActor(MonsterType.Get(), Location_ptr, Rotation_ptr, SpawnInfo);
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster; no monster type specified, ignoring."));
    }
}

void UMonsterGenerator::AutoSpawnMonster()
{
    // randomly pick a monster from the SpawnableMonsterClasses; if it's empty do nothing and log error.
    SpawnMonster(
        SpawnableMonsterClasses[
            FMath::RandRange(0, SpawnableMonsterClasses.Num()-1)
        ]
    );
}