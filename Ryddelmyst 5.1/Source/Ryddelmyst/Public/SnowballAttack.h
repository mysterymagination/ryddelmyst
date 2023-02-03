// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attack.h"
#include "SnowballAttack.generated.h"

/**
 * An Attack representing the damage formula and configurable effects/damage type of the Snowball spell
 */
UCLASS()
class RYDDELMYST_API USnowballAttack : public UAttack
{
	GENERATED_BODY()
	// Vector of functions to be run on the target after the spell hits
	std::vector<std::function<void(AActor* TargetActor, const FHitResult& HitResult)>> EffectsOnTarget;
	// Function that provides custom launch behavior
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> LaunchFn;
protected:
	/**
	 * @brief Cache of most recently calculated damage
	 * 
	 */
	UPROPERTY()
	float Damage = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TMap<TSubclassOf<UDamageType>, float> DamageTypesToWeightsMap;
	// base power used in CalculateDamageTx() to derive damage dealt by this spell
	UPROPERTY(EditAnywhere, Category = Projectile)
	float Power = 25.f;
	// scale factor for the final calculated damage
	UPROPERTY(EditAnywhere, Category = Projectile)
	float DamageScaleFactor = 1.f;
public:
	USnowballAttack();
	virtual void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
	// todo: need configurable damage types
	// todo: need configurable effects
	UFUNCTION()
	float GetDamageScaleFactor() { return DamageScaleFactor; };
	UFUNCTION()
	void SetDamageScaleFactor(float ScaleFactor) { DamageScaleFactor = ScaleFactor; };
	std::vector<std::function<void(AActor*, const FHitResult&)>>& GetEffectsVector() { return EffectsOnTarget; };
	std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> GetLaunchFunction() { return LaunchFn; };
	void SetLaunchFunction(std::function<void(AActor* LaunchingActor, const FVector& LaunchDirection)> Function) { LaunchFn = Function; };
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) override;
};
