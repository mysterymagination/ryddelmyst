// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RyddelmystGameInstance.h"
#include "RyddelmystGameMode.generated.h"

//enum to store the current state of gameplay
UENUM()
enum class EGamePlayState : uint8
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class ARyddelmystGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARyddelmystGameMode();
	virtual void StartPlay() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
	EGamePlayState GetCurrentState() const;
	/** Sets a new playing state */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentState(EGamePlayState NewState);
private:
	/**Keeps track of the current playing state */
	EGamePlayState CurrentState;
private:
	/**Handle any function calls that rely upon changing our gameplay state */
	void HandleNewState(EGamePlayState NewState);
	void HandlePlayerDeath();
};



