// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "RyddelmystGameState.h"
#include "Components/ScrollBox.h"
#include "ConversationStarter.generated.h"

/**
 * A parser for JSON conversation data that generates Slate UI elements from a conversation script, including interactive dialogue trees.
 * This class is also responsible for doing the reverse -- writing a conversation replay JSON script so the player can review past conversations in the quest log;
 * these would essentially look like the final conversation UI after dialogue options have been picked and the actual talking head + speech bubble
 * have been rendered. Basically a modified version of the original conversation script with prefab chosen dialogue options instead of the directives to
 * offer choice button UI.
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API UConversationStarter : public UObject
{
	GENERATED_BODY()

private:
	UConversationStarter();

	/**
	 * @brief Tries to match a known game character name token within a given actor's full GetName() result.
	 * @param ActorName - GetName() result for an actor in a conversation.
	 * @return matched character name or empty string. 
	 */
	UFUNCTION()
	FString MatchCharacter(const FString& ActorName);

	/**
	 * @brief Uses character and game state to figure out which specific script we need to load.
	 * @param CharacterName - the character's name such as MATCHER_YVYTEPH_MASTERMIND.
	 * @return best fit script name or empty string if none could be found. 
	 */
	UFUNCTION()
	FString CalculateScriptName(const FString& CharacterName);

	/**
	 * @brief Derives convo deadend behavior from the input clue e.g. yvyteph mastermind banishing Maya back to the starter table.
	 * @param Clue - a context clue string suggesting game behavior.
	 */
	UFUNCTION()
	void DeriveDeadend(const FString& Clue);

	/**
	 * @brief Inserts a divider graphic into the conversation container scrollbox.
	 */
	UFUNCTION()
	void AddDivider();

	/**
	 * @brief Scrolls to just below the end of current scrollbox plus a fudge factor derived by the height of the player dialogue that usually starts each convo segment.
	 * todo: why didn't any of the actual measured height params on UUserWidget work? 
	 * todo: use height of whatever actual first element of next segment will be?
	 */
	UFUNCTION()
	void PageDown();

	/**
	 * Installs the default exit conversation button behavior, e.g. closing out the conversation UI.
	 */
	UFUNCTION()
	void InstallDefaultExitBehavior();

	/**
	 * Runs the default exit conversation button behavior, e.g. closing out the conversation UI.
	 */
	UFUNCTION()
	void ExecuteDefaultExitBehavior();

private:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ConvoBaseWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> DialogueWidgetClass_Player;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> DialogueWidgetClass_Other;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ThoughtsWidgetClass_Player;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ThoughtsWidgetClass_Other;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ButtonWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> TextInputWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ChoicesWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> DividerWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	ARyddelmystGameState* GameState;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FString ConvoRx;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FString ConvoTx;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	FName ClosestBone;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	UUserWidget* ConvoWidget;
	
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	UScrollBox* ConvoContainer;

	/**
	 * @brief the scroll offset to apply beyond the old bottom point of the scrollbox when we jump to a new dialogue subtree; this will normally be the height of a DialogueWidgetClass_Player UUserWidget since conversation forks currently only come from the player asking questions. 
	 * todo: unfortunately the Slate API has foiled my several attempts to stat the dimensions of a UUserWidget accurately at runtime; DesignTimeSize gives me a wrong value and MinimumDesiredSize() gives me 0 and that is two attempts more than I have patience with re: UI. So fudge factor it shall be!
	 */
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	float SubtreeOffset = 500.f;

	TSharedPtr<FJsonObject> ScriptJsonObject{nullptr};

public:
	static const FString KEY_ARRAY_DIALOGUE;
	static const FString KEY_STRING_NAME;
	static const FString KEY_STRING_IMAGE;
	static const FString KEY_ARRAY_LINES;
	static const FString KEY_ARRAY_CHOICES;
	static const FString KEY_STRING_TEXT;
	static const FString KEY_ARRAY_THOUGHTS;
	static const FString KEY_OBJECT_DEADEND;
	static const FString KEY_OBJECT_JUMP;
	static const FString KEY_STRING_CLUE;
	static const FString KEY_STRING_INPUT;
	static const FString KEY_OBJECT_TRANSITION;
	static const FString KEY_STRING_TYPE;

	static const FString VALUE_TRANSITION_TYPE_JUMP;
	static const FString VALUE_TRANSITION_TYPE_DEADEND;

	static const FString MATCHER_YVYTEPH_FONTOFFERTILITY;
	static const FString MATCHER_YVYTEPH_GLORYFORM;
	static const FString MATCHER_YVYTEPH_WILDFORM;
	static const FString MATCHER_YVYTEPH_WILDFORMSHARP;
	static const FString MATCHER_YVYTEPH_MASTERMIND;
	static const FString MATCHER_QYVNILY_WILDFLOWER;
	static const FString MATCHER_QYVNILY_WILDFORM;
	static const FString MATCHER_QYVNILY_GLORYFORM;
	static const FString MATCHER_QYVNILY_GLORYFORMRAGE;
	static const FString MATCHER_TEST;

public:
	static FString PrettyTimestamp();
	/**
	 * @brief Returns the JSON string content of the conversation script that should be used.
	 * @param ConvoTx - the name of the Actor who instigated the conversation.
	 * @param ConvoRx - the name of the Actor who ConvoTx started a conversation with.
	 * @param ClosestBone - indicates where the player was looking when initiating the convo.
	 * @param GameState - handle to the current game state so that it can modify which conversation script is returned.
	 * @return the best fit conversation JSON script contents as an FString.
	 */
	void Init(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState);

	/**
	 * Uses the conversing character identities and game state set in Init() to determine the correct conversation script.
	 * @return the raw JSON string content of the most appropriate conversation script.
	 */
	FString GetScript();

	/**
	 * Parses the input script raw JSON string and generates a conversation UI for display. 
	 * @param Script - the raw JSON string of the convo script.
	 * @return a populated BP_ConvoBase widget for display in the HUD.
	 */
	UUserWidget* GenerateConversationUI(const FString& Script);

	/**
	 * @brief accepts a conversation script as a string and sets the CurrentScriptJsonObject field.
	 * @param Script - the raw JSON string of the convo script.
	 */
	void ParseConversationScript(const FString& Script);

	/**
	 * @brief parses the input convo JSON object and appends generated content to the given UWidget.
	 * @param DialogueObject - wrapper JsonObject hosting the current dialogue array.
	 */
	void ParseDialogue(TSharedPtr<FJsonObject> DialogueObject);

	/**
	 * @brief Steps through the ConvoWidget's widgettree and writes out conversation JSON capturing all the actual words spoken i.e. after choices were made or input was provided;
	 * essentially this produces a 'pre-rendered' conversation containing only the portraits, lines, and thoughts from the dialogue array elements. The format will match the interactive conversation
	 * scripts such that ParseConversationScript() can be used to load the pre-rendered conversations for review in the quest log.
	 * @param ConvoName - a unique name for this conversation instance, e.g. Qyvnily_WildForm_20241201T003000
	 */
	void SaveConversation(const FString& ConvoName);
	
};
