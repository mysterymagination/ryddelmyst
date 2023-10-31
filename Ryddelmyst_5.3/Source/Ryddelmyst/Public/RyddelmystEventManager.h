// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RyddelmystEventManager.generated.h"

/**
 * Home to Ryddelmyst game events that any Actor can register for and indirectly signal.
 */
UCLASS()
class RYDDELMYST_API URyddelmystEventManager : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Broadcasts whenever the Wood Egg is put in danger. 
	 */
	DECLARE_EVENT(URyddelmystEventManager, FWoodEggDangerEvent)
	FWoodEggDangerEvent& OnWoodEggEndangered() { return WoodEggDangerEvent; }
	/**
	 * Broadcasts whenever a player dies.
	 */
	DECLARE_EVENT(URyddelmystEventManager, FPlayerDeathEvent)
	FPlayerDeathEvent& OnPlayerDeath() { return PlayerDeathEvent; }
private:
	FWoodEggDangerEvent WoodEggDangerEvent;
	FPlayerDeathEvent PlayerDeathEvent;
};
