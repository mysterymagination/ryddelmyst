// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "ExtremityUnit.h"
#include "IAnatomy.h"
#include "LegCapsuleComponent.generated.h"

/**
 * A CapsuleComponent representing a creature's leg and processing hits delivered and received via the leg
 */
UCLASS(Blueprintable, BlueprintType)
class RYDDELMYST_API ULegCapsuleComponent : public UCapsuleComponent, public IAnatomy, public IAttacker, public IDefender 
{
	GENERATED_BODY()
	
	UPROPERTY()
	UExtremityUnit* LegUnit;

public: 
	UAnatomyUnit* GetAnatomyUnit_Implementation() {return LegUnit;}
	void OnHit_Implementation(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
	TArray<TSubclassOf<UDamageType>> GetDamageTypes_Implementation(const FString& AttackName) { return LegUnit->GetDamageTypes(); }
	float CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer) { return 0.f; }
	float CalculateDamageRx_Implementation(float BaseDamage, const TArray<TSubclassOf<UDamageType>>& IncomingDamageTypes) { return BaseDamage; }
	const TArray<TSubclassOf<UDamageType>> GetResistances() { TArray<TSubclassOf<UDamageType>> Resistances; return Resistances; }
	float GetResistanceFactor() { return 1.f; }
	const TArray<TSubclassOf<UDamageType>> GetVulnerabilities() { TArray<TSubclassOf<UDamageType>> Vulnerabilities; return Vulnerabilities; }
	float GetVulnerabilityFactor() { return 1.f; }
};
