// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RyddelmystEventManager.h"
#include "RyddelmystGameInstance.generated.h"

/**
 * GameInstance: high-level manager object for an instance of the running game. Spawned at game creation and not destroyed until game instance is shut down. Running as a standalone game, there will be one of these. Running in PIE (play-in-editor) will generate one of these per PIE instance.
 * RyddelmystGameInstance adds a RyddelmystEventManager UPROPERTY to manage events per game session.
 */
UCLASS()
class RYDDELMYST_API URyddelmystGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	URyddelmystGameInstance();
	UFUNCTION(BlueprintCallable, Category = "Event")
	URyddelmystEventManager* GetEventManager() { return EventManager; }
	UFUNCTION(BlueprintCallable, Category = "Sound")
	void AdjustAmbientSounds(float ScaleFactor);
private:
	UPROPERTY()
	URyddelmystEventManager* EventManager;
	/**
	 * @brief Maximum number of monsters allowed on the map at once; too many leads to lag. A better/complementary approach would be 
	 * map sectors and loading/unloading monsters nearby the player.
	 */
	UPROPERTY()
	int MaxMonsterCount = 25;
	/**
	 * @brief Current number of monsters spawned; each monster reports in at its BeginPlay() to increment this number and
	 * reports its destruction in HandleDeath().
	 */
	UPROPERTY()
	int CurrentMonsterCount = 0;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MusicVolumeScale = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float SFXVolumeScale = 0.5f;
};
