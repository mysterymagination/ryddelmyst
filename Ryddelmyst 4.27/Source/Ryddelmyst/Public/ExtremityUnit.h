// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnatomyUnit.h"
#include "ExtremityUnit.generated.h"

/**
 * A unit of anatomy that takes less damage than the core or head, but which may be crucial to an organism's locomotion or combat prowess;
 * damage to this unit may debilitate one or more of its stats.
 */
UCLASS()
class RYDDELMYST_API UExtremityUnit : public UAnatomyUnit
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool IsUsedForLocomotion = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	bool IsUsedForAttack = false;
	/**
	 * Cuts the relevant stats in half corresponding to the IsUsedForLocomotion (Speed) and IsUsedForAttack (Attack) members 
	 */
	void Debilitate(AActor* BattleStatsBearer);
};
