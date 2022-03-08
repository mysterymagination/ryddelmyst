// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystGameMode.h"
#include "RyddelmystHUD.h"
#include "RyddelmystCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

ARyddelmystGameMode::ARyddelmystGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Ryddelmyst_Assets/Blueprints/BP_Maya"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARyddelmystHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
}

void ARyddelmystGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is RyddelmystGameMode!"));
}

void ARyddelmystGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetCurrentState(EGamePlayState::EPlaying);
	PlayerCharacter = Cast<ARyddelmystCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay; player character set to %s"), PlayerCharacter ? *PlayerCharacter->GetName() : TEXT("null"));
}

void ARyddelmystGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerCharacter)
	{
		if (FMath::IsNearlyZero(PlayerCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
			HandleNewState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState ARyddelmystGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ARyddelmystGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
}

void ARyddelmystGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
		case EGamePlayState::EPlaying:
		{
			// do nothing
		}
		break;
		case EGamePlayState::EGameOver:
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
		}
		break;
		default:
		case EGamePlayState::EUnknown:
		{
			// do nothing
		}
		break;
	}
}
