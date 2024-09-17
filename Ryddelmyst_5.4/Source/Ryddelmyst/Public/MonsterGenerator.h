// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Monster.h"
#include "MonsterGenerator.generated.h"

/**
 * A component that spawns monsters parametrically from the approximate location of the owning Actor.
 * Spawning can occur on a clock, by event, or both.
 */
UCLASS(ClassGroup = "LevelDesign", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "MonsterGenerator", BlueprintSpawnableComponent))
class RYDDELMYST_API UMonsterGenerator : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FTimerHandle AutoSpawnTimerHandle;
	UPROPERTY()
	FTimerHandle AllowSpawnTimerHandle;
	/**
	 * @brief True if the generator is currently able to spawn a monster, false otherwise.
	 */
	UPROPERTY()
	bool AllowSpawn = true;
public:
	/**
	 * @brief True if the generator should spawn monsters automatically every SpawnPeriod seconds, false if the generator needs an event to trigger spawning.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	bool AutomaticSpawn = false;
	/**
	 * @brief Array containing the types of monsters this spawner can spawn. Queried when SpawnMonster is passed nullptr.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	TArray<TSubclassOf<AMonster>> SpawnableMonsterClasses;
	/**
	 * @brief The amount of time in seconds between automatic spawns, if AutomaticSpawn is enabled.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	float SpawnAutoPeriod = 30.f;
	/**
	 * @brief The amount of time in seconds that must pass between spawns, regardless of trigger. This prevents accidental over-spawn situations from
	 * e.g. colliding with a spawn trap tile repeatedly.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	float SpawnCooldownPeriod = 3.f;
public:
	/**
	 * @brief Spawns an instance of the given monster type nearby the generator.
	 * @param MonsterClass - the monster type to spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelDesign")
	void SpawnMonster(const TSubclassOf<AMonster> MonsterClass);
	/**
	 * @brief Spawns an instance of the given monster type nearby the generator.
	 * @param MonsterClass - the monster type to spawn.
	 * @param Transform - the spawn transform to use.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelDesign")
	void SpawnMonsterAt(const TSubclassOf<AMonster> MonsterClass, const FTransform& Transform);
	/**
	 * @brief Spawns a pattern of monsters nearby the generator given by the type:count in the input mapping.
	 * @param MonsterClassCountMap - a mapping of the monster types to the number of each type that should spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelDesign")
	void SpawnMonsterMap(const TMap<TSubclassOf<AMonster>, int>& MonsterClassCountMap);
	/**
	 * @brief Spawns a monster nearby the generator, choosing a random monster type from SpawnableMonsterClasses.
	 */
	UFUNCTION(BlueprintCallable, Category = "LevelDesign")
	void AutoSpawnMonster();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void SpawnCooled() { AllowSpawn = !AllowSpawn; };
};
