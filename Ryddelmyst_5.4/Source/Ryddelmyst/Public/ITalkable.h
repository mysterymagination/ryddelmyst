// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/GameStateBase.h"
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
	 * @param ClosestBone - indicates where the player was looking when initiating the convo.
	 * @param GameState - handle to the current game state so that it can modify which conversation script is returned.
	 * todo: for some reason I've got a butt bug about trying to do this 'right' by passing in game/player state or a 
	 * gameinstance subsystem or something (rather than just slapping a flag someplace common and having the talkable look it up), 
	 * since I have yet to actually use any of those in detail and the docs essentially
	 * skip them for some reason. Even the arch docs. My understanding is they (GameMode, GameState, and PlayerState) are mostly set dressing in the absence of networked gameplay 
	 * since in a single player game the client is the server and everybody can know everything, but I'd like to at least crack the mystery
	 * of how tf you actually USE e.g. a game state. Plus having things set up to extend into multiplayer is always nice.
	 * See https://forums.unrealengine.com/t/communicating-with-gamemode-gamestate-and-playerstate-in-a-networked-fps/650430
	 * for a link to community docs.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NPC")
	FString GetConversationScript(FName ClosestBone, AGameStateBase* GameState);
};
