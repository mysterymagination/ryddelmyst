// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RyddelmystGameState.generated.h"

/**
 * Tracks the current state of Maya's journey through the Megyle trial.
 */
UCLASS()
class RYDDELMYST_API ARyddelmystGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	bool WoodEggBeholden = false;
};
