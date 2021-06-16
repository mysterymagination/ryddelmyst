// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystGameMode.h"
#include "RyddelmystHUD.h"
#include "RyddelmystCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARyddelmystGameMode::ARyddelmystGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Ryddelmyst_Assets/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARyddelmystHUD::StaticClass();
}

void ARyddelmystGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is RyddelmystGameMode!"));
}
