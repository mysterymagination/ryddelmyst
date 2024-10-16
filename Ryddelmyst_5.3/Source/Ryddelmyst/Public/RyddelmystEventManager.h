// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RyddelmystEventManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWoodEggDangerEvent, bool, InDanger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeathEvent);

/**
 * Home to Ryddelmyst game events that any Actor can register for and indirectly signal.
 */
UCLASS()
class RYDDELMYST_API URyddelmystEventManager : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Broadcasts whenever the Wood Egg danger state changes; 
	 danger will be true if currently in danger (e.g. player picked it up) and false otherwise (e.g. player put it down).
	 */
	UPROPERTY(BlueprintAssignable)
	FWoodEggDangerEvent WoodEggDangerEvent;
	/**
	 * Broadcasts whenever a player dies.
	 */
	UPROPERTY(BlueprintAssignable)
	FPlayerDeathEvent PlayerDeathEvent;
};
