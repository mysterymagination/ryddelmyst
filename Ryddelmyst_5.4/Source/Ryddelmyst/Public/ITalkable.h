// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RyddelmystGameState.h"
#include "Blueprint/UserWidget.h"
#include "ITalkable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTalkable : public UInterface
{
	GENERATED_BODY()
};

/**
 * An NPC Actor who can be conversed with by the player and therefore knows how to return an appropriate conversation script.
 */
class RYDDELMYST_API ITalkable
{
	GENERATED_BODY()

public:
	/**
	 * @brief Returns the JSON string content of the conversation script that should be used.
	 * @param ConvoTx - the name of the Actor who instigated the conversation.
	 * @param ConvoRx - the name of the Actor who ConvoTx started a conversation with.
	 * @param ClosestBone - indicates where the player was looking when initiating the convo.
	 * @param GameState - handle to the current game state so that it can modify which conversation script is returned.
	 * @return the best fit conversation JSON script contents as an FString.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NPC")
	FString GetConversationScript(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState);
	/**
	 * @brief Processes the input conversation script JSON and renders a conversation UI.
	 * @param ConvoTx - the name of the Actor who instigated the conversation.
	 * @param ConvoRx - the name of the Actor who ConvoTx started a conversation with.
	 * @param ClosestBone - indicates where the player was looking when initiating the convo.
	 * @param GameState - handle to the current game state so that it can modify which conversation script is returned.
	 * @param ConvoJSON - optional json script as FString that details the conversation flow and options; 
	 * if not specified, we will call GetConversationScript() to figure out the script content. 
	 * @return UUserWidget containing the rendered conversation in a container suitable for replacing or appending to a conversation container in the HUD.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NPC")
	UUserWidget* StartConversation(const FString& ConvoTx, const FString& ConvoRx, FName ClosestBone, ARyddelmystGameState* GameState, const FString& ConvoJSON);
};
