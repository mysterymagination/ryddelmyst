// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterAI.h"
#include "BattleStats.h"
#include "BattleStatsBearer.h"
#include "StatusEffected.h"
#include "HitBoxerComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	UHitBoxerComponent* HitBoxer;

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
	float RunSpeedFactor = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	UNiagaraSystem* DeathParticleSystem;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AMonster();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UBattleStats* GetStats_Implementation() { return MonsterStats; }
	UFUNCTION(BlueprintCallable, Category = Movement)
	bool GetRunningStatus();
	UFUNCTION(BlueprintCallable, Category = Movement)
	void Walk();
	UFUNCTION(BlueprintCallable, Category = Movement)
	void Run();
	UFUNCTION(BlueprintCallable, Category = Movement) 
	void ToggleRun();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void HandleDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	/**
	 * Defines behavior upon HP reaching 0; default is for the Actor to simply Destroy() itself, but Blueprints can customize. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void HandleDeath();
	virtual void HandleDeath_Implementation();
	// todo: refactor and create a common Character subclass for both Maya and monsters, with the common functionality like this so we can avoid copy/paste.  TBD what reparenting like that will do to my poor idiot blueprints, but it will be painful.
	void HandleStatModification_Implementation(const FString& StatName) 
	{ 
		if (StatName.Equals("Speed"))
		{
			GetCharacterMovement()->MaxWalkSpeed = IsRunning ? MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed * RunSpeedFactor : MonsterStats->StatsData.StatsMap["Speed"] * BaseWalkSpeed;
			UE_LOG(LogTemp, Warning, TEXT("UpdateSpeed; monster max walk speed became %f from speed factor %f times BaseWalkSpeed %f %s"),
				GetCharacterMovement()->MaxWalkSpeed, MonsterStats->StatsData.StatsMap["Speed"], BaseWalkSpeed, (IsRunning ? *FString::Printf(TEXT("and running factor of %f"), RunSpeedFactor) : *FString(TEXT(""))));
		}
	}
	void AddStatusEffect_Implementation(UStatusEffect* Effect) 
	{ 
		StatusEffects.Add(Effect); 
	}
	void RemoveStatusEffect_Implementation(const FString& EffectId)
	{ 
		const UStatusEffect* StatusToRemove = *StatusEffects.FindByPredicate([&](const UStatusEffect* Effect)
			{
				return Effect->GetName().Equals(EffectId);
			}
		); 
		StatusEffects.RemoveAt(StatusEffects.Find(StatusToRemove));
	}	
};
