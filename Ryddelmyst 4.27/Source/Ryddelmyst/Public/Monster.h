// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterAI.h"
#include "BattleStats.h"
#include "BattleStatsBearer.h"
#include "StatusEffected.h"
#include "Monster.generated.h"

/**
 * A Character controlled by MonsterAI
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API AMonster : public ACharacter, public IBattleStatsBearer, public IStatusEffected
{
	GENERATED_BODY()
	UPROPERTY()
	UBattleStats* MonsterStats;
	UPROPERTY()
	TArray<const UStatusEffect*> StatusEffects;
	UPROPERTY()
	bool IsRunning = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TSubclassOf<UBattleStats> MonsterStatsType;

public:
	/**
	 * @brief The cm/s walking speed we use for character base speed
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BaseWalkSpeed = 300.f;

	/**
	 * @brief The scaling factor by which running increases base speed
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float RunSpeedFactor = 10.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AMonster();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UBattleStats* GetStats_Implementation() { UE_LOG(LogTemp, Warning, TEXT("GetStats; getting monster stats")); return MonsterStats; }
	// todo: refactor and create a common Character subclass for both Maya and monsters, with the common functionality like this so we can avoid copy/paste.  TBD what reparenting like that will do to my poor idiot blueprints, but it will be painful.
	void UpdateSpeed_Implementation() 
	{ 
		GetCharacterMovement()->MaxWalkSpeed =  IsRunning ? MonsterStats->Speed * BaseWalkSpeed * RunSpeedFactor : MonsterStats->Speed * BaseWalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("UpdateSpeed; monster max walk speed became %f from speed factor %f times BaseWalkSpeed %f %s"), 
			GetCharacterMovement()->MaxWalkSpeed, MonsterStats->Speed, BaseWalkSpeed, (IsRunning ? *FString::Printf(TEXT("and running factor of %f"), RunSpeedFactor) : *FString(TEXT(""))));
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
