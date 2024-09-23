// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RyddelmystGameState.generated.h"

/**
 * enum to store the current state of Maya's journey through the trials
 */
UENUM()
enum class EMegyleTrialFlags : uint8
{
	EMegReveal,
	EDiaryID,
	EUnknown
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	TMap<FString, bool> StatesMapBool = { 
		{TEXT("Read_Diary_BlastoffEnthusiasm1"), false},
		{TEXT("Read_Diary_Signature"), false},
		{TEXT("Yvyteph_Mentioned_Daughter"), false}
	};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	TMap<FString, int> StatesMapInt = {
		{TEXT("Molly_Glances"), 0},
		{TEXT("Qyvnily_Forms_Visited"), 0}
	};
};
