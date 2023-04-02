// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAI.generated.h"

/**
 * AI Controller intended for general Ryddelmyst monster behavior
 */
UCLASS()
class RYDDELMYST_API AMonsterAI : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(ViewAnywhere, BlueprintReadOnly, Category = AI)
	FTimerHandle TrackingTimerHandle;
	UPROPERTY(ViewAnywhere, BlueprintReadOnly, Category = AI)
	FTimerHandle TeleportTimerHandle;
	/**
	 * @brief The period at which we refresh our tracking behavior, in seconds.  If the AI finds the player and stops tracking, it will need to wait until this period is up to resume the chase.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float TrackingPeriod = 1.f;
	/**
	 * @brief The period at which we teleport near the player, in seconds.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float TeleportPeriod = 30.f;
	/**
	 * @brief The vector offset from the player which determines the location we teleport to.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	FVector TeleportOffset;

protected:
	virtual void BeginPlay() override;

public: 
	AMonsterAI();
	void TrackPlayer();
	void TeleportNearPlayer();
};
