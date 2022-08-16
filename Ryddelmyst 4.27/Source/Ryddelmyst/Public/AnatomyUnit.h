// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AnatomyUnit.generated.h"

/**
 * Base class for a unit of an organism's anatomy, such as arms, legs, or head.  Processes incoming damage from attacks based on the specific role of the body part
 * with customization options for different types of creatures.
 */
UCLASS()
class RYDDELMYST_API UAnatomyUnit : public UObject
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
	
private:
	/**
	 * @brief Calculates the damage received from the given base damage, e.g. hit to the arm might be reduced to base damage * 0.25f
	 * @param BaseDamage the base damage the incoming hit inflicts before anatomy is taken into account
	 * @return the rx damage adjusted for anatomy
	 */
	/* todo: this should go in an interface IDefender (and CalculateDamageTx() will go in IAttacker)
	UFUNCTION()
	float CalculateDamageRx(float BaseDamage);
	*/
	/**
	 * @brief Applies a timed BattleStats modification to the damaged AActor based on the stricken anatomy e.g. a stricken leg may reduce speed by half
	 * 
	 */
	UFUNCTION()
	void Debilitate(){};
};
