// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "MonsterAI.h"

AMonsterAI::AMonsterAI() 
:
TeleportOffset(100.f, 0.f, 0.f)
{
    PrimaryActorTick.bCanEverTick = true;
}

void AMonsterAI::BeginPlay()
{
    Super::BeginPlay();
    GetWorldTimerManager().SetTimer(TrackingTimerHandle, this, &AMonsterAI::TrackPlayer, TrackingPeriod, true);
    GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &AMonsterAI::TeleportNearPlayer, TeleportPeriod, true);
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

void AMonsterAI::TeleportNearPlayer()
{
    // generate random angle in degrees for the teleport offset vector to be rotated by; this will cause the treant to approach the player from different directions after teleporting, and by design occasionally show up in the woodwork just to jumpscare 
    float RandAngle = FMath::RandRange(0.f, 360.f);
    FRotator OffsetRot = FRotator(0.f, RandAngle, 0.f);
    FVector ScaryOffset = OffsetRot.RotateVector(TeleportOffset);
    AActor* PursuedActor = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    GetCharacter()->SetActorLocation(PursuedActor->GetActorLocation() + ScaryOffset);
    // todo: play a creepy noise when he teleports
}