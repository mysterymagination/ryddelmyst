// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SplineGuideComponent.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "TimerManager.h"
#include "RyddelmystEventManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWoodEggDangerEvent, bool, InDanger);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWoodEggDeathEvent, FVector, DeathLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSplineGuideCompletionEvent, USplineGuideComponent*, InCompletedSplineGuideComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeathEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompletionEvent, const FString&, QuestCompleteContext);

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
	 * Broadcasts when the Wood Egg is destroyed i.e when the player drops it in the lava pool.
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadOnly)
	FWoodEggDeathEvent WoodEggDeathEvent;
	/**
	 * Broadcasts whenever a player dies.
	 */
	UPROPERTY(BlueprintAssignable)
	FPlayerDeathEvent PlayerDeathEvent;
	/**
	 * Broadcasts whenever a USplineGuideComponent completes, passing a pointer to the relevant USplineGuideComponent instance.
	 */
	UPROPERTY(BlueprintAssignable)
	FSplineGuideCompletionEvent SplineGuideCompletionEvent;

	/**
	 * Broadcasts when the player has satisfied an endgame condition e.g. slaying the Reaper Treant.
	 * @param QuestCompleteContext - a string explaining the manner in which endgame was reached e.g. craving_queen
	 */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, BlueprintReadOnly)
	FQuestCompletionEvent QuestCompletionEvent;

private:
	UPROPERTY()
	FTimerHandle ScheduledEventTimerHandle;

public:
	/**
 	 * Schedule quest complete event broadcast to run at the given delay
	 * @param ActorInWorld - AActor from whom we'll derive the the FTimerManager for the world. UFUNCTION doesn't like FTimerManager for some reason.
	 * @param DelaySeconds - timer delay in seconds.
	 * @param CompletionContext - FString explaining how/why the quest was completed.
 	 */
	UFUNCTION(BlueprintCallable)
	void ScheduleQuestCompletionEvent(AActor* ActorInWorld, float DelaySeconds, const FString& CompletionContext);
};
