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
	FTimerHandle SpawnTimerHandle;
public:
	/**
	 * @brief True if the generator should spawn monsters automatically every SpawnPeriod seconds, false if the generator needs an event to trigger spawning.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	bool AutomaticSpawn;
	/**
	 * @brief Array containing the types of monsters this spawner can spawn. Queried when SpawnMonster is passed nullptr.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	TArray<TSubclassOf<AMonster>> SpawnableMonsterClasses;
	/**
	 * @brief The amount of time in seconds between spawns.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	float SpawnPeriod;
public:
	/**
	 * @brief Spawns an instance of the given monster type nearby the generator.
	 * @param MonsterClass - the monster type to spawn.
	 */
	void SpawnMonster(const TSubclassOf<AMonster> MonsterClass);
	/**
	 * @brief Spawns a pattern of monsters nearby the generator given by the type:count in the input mapping.
	 * @param MonsterClassCountMap - a mapping of the monster types to the number of each type that should spawn.
	 */
	void SpawnMonster(const TMap<TSubclassOf<AMonster>, int>& MonsterClassCountMap);
	/**
	 * @brief Spawns a monster nearby the generator, choosing a random monster type from SpawnableMonsterClasses.
	 */
	void AutoSpawnMonster();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
