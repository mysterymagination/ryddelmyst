// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RyddelmystGameMode.generated.h"

UCLASS(minimalapi)
class ARyddelmystGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARyddelmystGameMode();
	void StartPlay() override;
};



