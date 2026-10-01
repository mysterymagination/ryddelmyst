// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RyddelmystEventManager.h"
#include "Monster.h"
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
	/**
	 * @brief Check to see if the current gameinstance can support more monsters at the moment.
	 * @param MonsterType - the monster type we want to spawn; this is presently ignored in favor of simply checking the total number of monsters spawned.
	 * @return true if the game instance supports spawning another monster, false otherwise e.g. if max monster count has been reached.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelDesign")
	bool CanSpawnMonster(TSubclassOf<AMonster> MonsterType);
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void Pause();
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void Unpause();
private:
	UPROPERTY()
	URyddelmystEventManager* EventManager;
	/**
	 * @brief Maximum number of monsters allowed on the map at once; too many leads to lag. A better/complementary approach would be 
	 * map sectors and loading/unloading monsters nearby the player.
	 */
	UPROPERTY()
	int MaxMonsterCount = 15;
public:
	static const FString TAG_FLAG_IGNORE_USER_VOL;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float MusicVolumeScale = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float SFXVolumeScale = 0.5f;

	/**
	 * @brief Current number of monsters spawned; each monster reports in at its BeginPlay() to increment this number and
	 * reports its destruction in HandleDeath().
	 */
	UPROPERTY()
	int CurrentMonsterCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locations")
	FVector PlayerStartPosition{-740.f, -509.f, 348.f};
};
