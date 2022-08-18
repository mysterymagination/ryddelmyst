// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IAttacker.h"
#include "IDefender.h"
#include "AnatomyUnit.generated.h"

/**
 * Base class for a unit of an organism's anatomy, such as arms, legs, or head.  Processes incoming damage from attacks based on the specific role of the body part
 * with customization options for different types of creatures.
 */
UCLASS()
class RYDDELMYST_API UAnatomyUnit : public UObject, public IAttacker, public IDefender
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief applies damage and possibly status effects depending on the particular anatomy unit subclass
	 * @param HittingComponent the UPrimitiveComponent that is the instigator of the collision
	 * @param HitActor the AActor being hit
	 * @param HitComp the UPrimitiveComponent of the HitActor that is specifically being hit by the HittingComponent
	 * @param NormalImpulse impulse vector of the HittingComponent
	 * @param HitInfo FHitResult data
	 */
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void ProcessHit(UPrimitiveComponent* HittingComp, AActor* HitActor, UPrimitiveComponent* HitComp, FVector NormalImpulse, const FHitResult& HitInfo);
	
protected:
	/**
	 * @brief Damage types dealt by this (absolute) unit of anatomy, e.g. Bludgeoning for kicks/stomps or Slashing for talons
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TArray<TSubclassOf<UDamageType>> DamageTypes;
	const TArray<TSubclassOf<UDamageType>> GetDamageTypes_Implementation() { return DamageTypes; }
	float CalculateDamageTx_Implementation(const FString& AttackName, AActor* BattleStatsBearer) {return 0.f;}
	float CalculateDamageRx_Implementation(float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes) {return BaseDamage;}
	const TArray<TSubclassOf<UDamageType>> GetResistances() {TArray<TSubclassOf<UDamageType>> Resistances; return Resistances;}
	float GetResistanceFactor() {return 1.f;}
	const TArray<TSubclassOf<UDamageType>> GetVulnerabilities() {TArray<TSubclassOf<UDamageType>> Vulnerabilities; return Vulnerabilities;}
	float GetVulnerabilityFactor() {return 1.f;}
private:
	/**
	 * @brief Applies a timed BattleStats modification to the damaged AActor based on the stricken anatomy e.g. a stricken leg may reduce speed by half
	 * 
	 */
	UFUNCTION()
	void Debilitate(){};
};
