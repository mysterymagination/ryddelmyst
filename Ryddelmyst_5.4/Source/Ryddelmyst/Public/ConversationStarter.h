// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "RyddelmystGameState.h"
#include "ConversationStarter.generated.h"

/**
 * A parser for JSON conversation data that generates Slate UI elements from a conversation script, including interactive dialogue trees.
 * This class is also responsible for doing the reverse -- writing a conversation replay JSON script so the player can review past conversations in the quest log;
 * these would essentially look like the final conversation UI after dialogue options have been picked and the actual talking head + speech bubble
 * have been rendered. Basically a modified version of the original conversation script with prefab chosen dialogue options instead of the directives to
 * offer choice button UI.
 */
UCLASS()
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
	 * @param Character - the character's name such as MATCHER_YVYTEPH_MASTERMIND.
	 * @return best fit script name or empty string if none could be found. 
	 */
	UFUNCTION()
	FString CalculateScriptName(const FString& Character, ARyddelmystGameState* GameState);

private:
	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ConvoBaseWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> DialogueWidgetClass_Player;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> DialogueWidgetClass_Other;

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	TSubclassOf<UUserWidget> ChoicesWidgetClass;

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

	static const FString MATCHER_YVYTEPH_FONTOFFERTILITY;
	static const FString MATCHER_YVYTEPH_GLORYFORM;
	static const FString MATCHER_YVYTEPH_WILDFORM;
	static const FString MATCHER_YVYTEPH_WILDFORMSHARP;
	static const FString MATCHER_YVYTEPH_MASTERMIND;
	static const FString MATCHER_QYVNILY_WILDFLOWER;
	static const FString MATCHER_QYVNILY_WILDFORM;
	static const FString MATCHER_QYVNILY_GLORYFORM;
	static const FString MATCHER_QYVNILY_GLORYFORMRAGE;

public:
	/**
	 * @brief Returns the JSON string content of the conversation script that should be used.
	 * @param ConvoTx - the name of the Actor who instigated the conversation.
	 * @param ConvoRx - the name of the Actor who ConvoTx started a conversation with.
	 * @param ClosestBone - indicates where the player was looking when initiating the convo.
	 * @param GameState - handle to the current game state so that it can modify which conversation script is returned.
	 * @return the best fit conversation JSON script contents as an FString.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	FString GetScript(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState);

	/**
	 * @brief accepts a conversation script as a string and returns a UUserWidget populated by generated conversation UI elements
	 * @param Script - the raw JSON string of the convo script.
	 * @param GameState - the current game state, used to modify generated conversation and so that the script can set 'bookmark' variables like clue for further processing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	UUserWidget* ParseConversationScript(const FString& Script, ARyddelmystGameState* GameState);

	/**
	 * @brief parses the input convo JSON object and appends generated content to the given UWidget.
	 * @param ConvoWidget - the top level UUserWidget whose WidgetTree all our generated UWidgets belong to.
	 * @param Container - the UWidget that should host the UWidgets we generate from the convo JSON.
	 * @param DialogueElementsArray - the current convo dialogue as a JSON array.
	 * @param GameState - the current game state, used to modify generated conversation and so that the script can set 'bookmark' variables like clue for further processing.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ParseDialogue(UUserWidget* ConvoWidget, UWidget* Container, TArray<TSharedPtr<FJsonValue>>& DialogueElementsArray, ARyddelmystGameState* GameState);
	
};
