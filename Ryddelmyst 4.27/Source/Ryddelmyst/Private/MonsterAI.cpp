// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "MonsterAI.h"

AMonsterAI::AMonsterAI()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAI::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(TrackingTimerHandle, this, &AMonsterAI::TrackPlayer, TrackingPeriod, true);
   
}

void AMonsterAI::TrackPlayer()
{
    UE_LOG(LogTemp, Warning, TEXT("TrackPlayer; refreshing player track behavior"));
    AActor* PursuedActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    // turn towards player 1
    FRotator LookAtRotation = (PursuedActor->GetActorLocation() - GetCharacter()->GetActorLocation()).Rotation();
    GetCharacter()->SetActorRotation(LookAtRotation);
    // pursue player 1
    MoveToActor(
        PursuedActor,
        100.f,
        true,
        true,
        false,
        nullptr,
        true
    );
}
