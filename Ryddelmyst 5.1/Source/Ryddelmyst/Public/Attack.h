// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Attack.generated.h"

/**
 * An Attack represents a particular attack that can be performed with a Weapon; it provides data and logic necessary to calculate damage and deliver additional effects to the stricken target.
 */
UCLASS(ClassGroup = "Combat", editinlinenew, Blueprintable, BlueprintType, meta = (DisplayName = "Attack", BlueprintSpawnableComponent))
class RYDDELMYST_API UAttack : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FString AttackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BasePower = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DamageScaleFactor = 1.f;
public:
	/**
	 * @brief Processes a hit by this attacker on the given defender
	 * @param StrikingComp the component doing the hitting; this is the IAttacker impl
	 * @param StrickenActor the AActor we hit
	 * @param StrickenComp the UPrimitiveComponent we hit; this is the IDefender impl
	 * @param NormalImpulse the impulse vector normal to our strike point
	 * @param HitInfo collision data
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	/**
	 * @brief Calculates the base damage of our attack from the stats of the input IBattleStatsBearer implementor
	 * @param BattleStatsBearer the instigator of the attack, whose stats determine its damage output
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
	float CalculateDamageTx(AActor* BattleStatsBearer);
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) { return 0.f; }
};
