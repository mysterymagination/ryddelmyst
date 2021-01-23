// Copyright Epic Games, Inc. All Rights Reserved.

#include "RyddelmystGameMode.h"
#include "RyddelmystHUD.h"
#include "RyddelmystCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARyddelmystGameMode::ARyddelmystGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARyddelmystHUD::StaticClass();
}
