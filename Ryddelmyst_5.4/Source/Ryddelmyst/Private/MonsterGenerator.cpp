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

void UMonsterGenerator::SpawnMonster(const TSubclassOf<AMonster> MonsterType)
{
    if (MonsterType)
    {
        // spawn an instance of the given monster type at the location of the 
        // owning actor of the generator.
        FVector Location = GetOwner()->GetActorLocation();
        FRotator Rotation = GetOwner()->GetActorRotation();
        FTransform SpawnTransform;
		SpawnTransform.SetIdentity();
        SpawnTransform.SetTranslation(Location);
        SpawnTransform.SetRotation(FQuat(Rotation));
        SpawnTransform.SetScale3D(FVector(1.f));
        SpawnMonsterAt(MonsterType, SpawnTransform);
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonster; no monster type specified, ignoring."));
    }
}

void UMonsterGenerator::SpawnMonsterAt(const TSubclassOf<AMonster> MonsterType, const FTransform& Transform)
{
    if (MonsterType)
    {
        AMonster* Monster = GetWorld()->SpawnActorDeferred<AMonster>(
            MonsterType.Get(), 
            Transform, 
            nullptr, 
            nullptr, 
            ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
        );
        Monster->AutoPossessAI = EAutoPossessAI::Spawned;
        Monster->FinishSpawning(Transform);
    }
    else 
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonsterAt; no monster type specified, ignoring."));
    }
}

void UMonsterGenerator::SpawnMonsterMap(const TMap<TSubclassOf<AMonster>, int>& MonsterClassCountMap)
{
    for (auto& Elem : MonsterClassCountMap)
    {
        for (int i = 0; i < Elem.Value; i++)
        {
            SpawnMonster(Elem.Key);
        }
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