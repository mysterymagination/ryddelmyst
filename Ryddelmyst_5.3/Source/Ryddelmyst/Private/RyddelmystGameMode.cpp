// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystGameMode.h"
#include "RyddelmystHUD.h"
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
	// register for player death event 
	Cast<URyddelmystGameInstance>(GetWorld()->GetGameInstance())->GetEventManager()->OnPlayerDeath().AddUObject(this, &ARyddelmystGameMode::HandlePlayerDeath);
}

void ARyddelmystGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EGamePlayState ARyddelmystGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ARyddelmystGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
}

void ARyddelmystGameMode::HandlePlayerDeath()
{
	HandleNewState(EGamePlayState::EGameOver);
}

void ARyddelmystGameMode::HandleNewState(EGamePlayState NewState)
{
	CurrentState = NewState;
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
			// todo: add game over UI screen, maybe a nice 'you died' kinetic text animation
			// todo: take controls away from player, similar to how hitting the kill plane does
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
