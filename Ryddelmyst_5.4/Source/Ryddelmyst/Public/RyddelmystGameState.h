// Copyright Jeff Creswell 2024. Subject to the GPL v3.0 license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
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
class RYDDELMYST_API ARyddelmystGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	bool WoodEggBeholden = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	FString ClueState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	FString UserInputValue;
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
	static const FString STATE_CLUE_INPUT_CRUELTY_QUERY;
	static const FString STATE_CLUE_INPUT_TEST_ANSWER1;
	static const FString VALUE_INPUT_EGG_TOKEN;
	static const FString VALUE_INPUT_LAVA_TOKEN;
};
