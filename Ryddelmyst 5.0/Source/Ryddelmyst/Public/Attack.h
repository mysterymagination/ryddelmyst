// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IAttacker.h"
#include "Attack.generated.h"

/**
 * An Attack represents a particular attack that can be performed with a Weapon; it provides data and logic necessary to calculate damage and deliver additional effects to the stricken target.
 * Attacks implement IAttacker such that we can have reusable objects with common default behavior logic whose results are modified by parameterized instances. 
 */
UCLASS()
class RYDDELMYST_API UAttack : public UObject, IAttacker
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	FString AttackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG")
	float BasePower;
public:
	void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
    TArray<TSubclassOf<UDamageType>> GetDamageTypes_Implementation() { return DamageTypes; }
    float CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer) { return 0.f; }
};
