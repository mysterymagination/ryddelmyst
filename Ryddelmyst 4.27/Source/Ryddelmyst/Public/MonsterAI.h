// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterAI.generated.h"

/**
 * 
 */
UCLASS()
class RYDDELMYST_API AMonsterAI : public AAIController
{
	GENERATED_BODY()
	UPROPERTY()
	FTimerHandle TrackingTimerHandle;

protected:
	/**
	 * @brief The period at which we refresh our tracking behavior, in seconds.  If the AI finds the player and stops tracking, it will need to wait until this period is up to resume the chase.
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	float TrackingPeriod = 1.f;

protected:
	virtual void BeginPlay() override;

public: 
	AMonsterAI();
	void TrackPlayer();
	
	
};
