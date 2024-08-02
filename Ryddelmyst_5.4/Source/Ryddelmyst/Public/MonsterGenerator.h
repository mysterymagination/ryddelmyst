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
class RYDDELMYST_API MonsterGenerator : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
public:
	MonsterGenerator();
	/**
	 * @brief Spawns a monster nearby the generator.
	 * @param MonsterClass - the type of monster to spawn; if nullptr, then a random monster type from SpawnableMonsterClasses will be chosen.
	 */
	SpawnMonster(TSubclassOf<AMonster> MonsterClass);
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
	 * @brief The amount of time between spawns.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelDesign")
	float SpawnPeriod;
};
