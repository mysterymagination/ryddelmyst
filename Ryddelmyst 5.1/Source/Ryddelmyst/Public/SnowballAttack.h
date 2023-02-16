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
protected:
	/**
	 * @brief Cache of most recently calculated damage
	 * 
	 */
	UPROPERTY()
	float Damage = 0.f;
	/**
	 * @brief base power of the attack, used to derive the final damage to be transmitted
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magic)
	float Power = 0.f;
public:
	USnowballAttack();
	virtual void OnHit_Implementation(AActor* StrikingBattler, UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo) override;
	UFUNCTION()
	float GetDamageScaleFactor() { return DamageScaleFactor; };
	UFUNCTION()
	void SetDamageScaleFactor(float ScaleFactor) { DamageScaleFactor = ScaleFactor; };
	std::vector<std::function<void(AActor*, const FHitResult&)>>& GetEffectsVector() { return EffectsOnTarget; };
	virtual float CalculateDamageTx_Implementation(AActor* BattleStatsBearer) override;
};
