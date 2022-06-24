// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MonsterAI.h"
#include "BattleStats.h"
#include "BattleStatsBearer.h"
#include "StatusEffected.h"
#include "Monster.generated.h"

/**
 * A Pawn controlled by MonsterAI
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API AMonster : public APawn, public IBattleStatsBearer, public IStatusEffected
{
	GENERATED_BODY()
	UPROPERTY()
	UBattleStats* MonsterStats;
	UPROPERTY()
	TArray<const UStatusEffect*> StatusEffects;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TSubclassOf<UBattleStats> MonsterStatsType;

public:
	UBattleStats* GetStats_Implementation() { UE_LOG(LogTemp, Warning, TEXT("GetStats; getting monster stats")); return MonsterStats; }
	void UpdateSpeed_Implementation() 
	{ 
		// todo: figure out speed for a generic MovementComponent and/or make a custom monster movementcomponent
		UE_LOG(LogTemp, Warning, TEXT("UpdateSpeed; not yet implemented for monster")); 
	}
	void AddStatusEffect_Implementation(UStatusEffect* Effect) 
	{ 
		StatusEffects.Add(Effect); 
	}
	void RemoveStatusEffect_Implmenetation(const FString& EffectId) 
	{ 
		const UStatusEffect* StatusToRemove = *StatusEffects.FindByPredicate([&](const UStatusEffect* Effect)
			{
				return Effect->GetName().Equals(EffectId);
			}
		); 
		StatusEffects.RemoveAt(StatusEffects.Find(StatusToRemove));
	}	
	
	
};
