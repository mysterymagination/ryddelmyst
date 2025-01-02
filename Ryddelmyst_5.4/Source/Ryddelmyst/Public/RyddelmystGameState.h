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
	// todo: would be nice to have a mapping of input tokens to direct dialogue scripts or just text replies that can be wrapped in a UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	TArray<FString> MatingDanceTokens = {
		TEXT("pretty"),
		TEXT("beautiful"),
		TEXT("gorgeous"),
		TEXT("love"),
		TEXT("dance")
	};
	static const FString STATE_CLUE_INPUT_CRUELTY_QUERY;
	static const FString STATE_CLUE_INPUT_TEST_ANSWER1;
	static const FString STATE_CLUE_INPUT_QYVNILY_DANCE;
	static const FString STATE_CLUE_QYVNILY_WILD_PERSPECTIVE;
	static const FString STATE_CLUE_QYVNILY_GLORY_PERSPECTIVE;
	static const FString STATE_CLUE_YVYTEPH_UNDERCARRIAGE_CHECK_FAIL;
	static const FString STATE_CLUE_YVYTEPH_MASTERMIND_REJECTED_ANGEL;
	static const FString STATE_CLUE_MAYA_EVIL_DETERMINATION;
	static const FString STATE_CLUE_ENDING_CRAVING_QUEEN_HOMEWARD;
	static const FString STATE_CLUE_ENDING_CRAVING_QUEEN_HEAVENWARD;
	static const FString STATE_CLUE_ENDING_AMOROUS_ANGEL_HOMEWARD;
	static const FString STATE_CLUE_ENDING_AMOROUS_ANGEL_HEAVENWARD;
	static const FString STATE_CLUE_ENDING_AMOROUS_ANGEL;
	static const FString STATE_CLUE_ENDING_CRAVING_QUEEN;
	static const FString STATE_CLUE_ENDING_PRACTICAL_PAWN;
	static const FString VALUE_INPUT_EGG_TOKEN;
	static const FString VALUE_INPUT_LAVA_TOKEN;
};
