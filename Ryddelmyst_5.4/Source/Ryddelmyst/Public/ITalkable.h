// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RyddelmystGameState.h"
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
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "NPC")
	FString GetConversationScript(FName ClosestBone, ARyddelmystGameState* GameState);
};
