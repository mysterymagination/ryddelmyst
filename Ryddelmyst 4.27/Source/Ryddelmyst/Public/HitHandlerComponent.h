// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnatomyUnit.h"
#include "HitHandlerComponent.generated.h"

/**
 * @brief Contains logic for handling different types of hits based on this component's property config e.g. handling hits to an arm may do reduced damage but reduce the target's Attack stat
 * vs. handling hits to a head may do double damage
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RYDDELMYST_API UHitHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// todo: should this maybe be an array of UShapeComponent subclasses like we make a UClawComponent or something that subclasses UCapsuleComponent so that we can install actual claws on
	//  the treant and other monsters and then have the player be vulnerable to claws, teeth, hooves etc. (and maybe some monsters as well, for in-fighting?).TRhese could obvs be customizeable
	//  per-instance with base damage etc. and take a reference to the host's BattleStats.  I'm looking for a way to avoid 
	//  having a string lookup or a cast or something to id if we're looking at a player or a monster and then hardcoding a lookup of ASnowball's damage or a great big switch statement of ways
	//  to look up monster damage sources.  Another way might be to put a CalculateDamage(FString Attackname) on Monster.h, but then we're talking about potentially more code with less coherence
	//  for less benefit; in that case we'd still need to know if we're dealing with a player or monster as the damaging AActor (or at least whether or not we're being hit by a ASnowball since
	//  that is the player's only means of attack).  I kinda like the notion of encapsulating the attack all the way down in the attacking anatomy, effectively, with form classes that subclass UShapeComponent
	//  to work beside our function UAnatomyUnit classes.  
	/**
	 * @brief Array of recognized damage sources e.g. ASnowball objects damage the treant and the treant's claws damage the player.  For simplicity and versatility, we'll use simple key strings
	 * that will be substrings of the names of UPrimitiveComponents involved in the collision. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	TArray<FString> Vulnerabilities;
	/**
	 * @brief Mapping of anatomy identifiers to UAnatomyUnit objects; when e.g. and arm's hitbox is hit, its oncomponenthit event can lookup the relevant UExtremityUnit object for 
	 * that arm and call its ProcessHit()
	 * 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	TMap<FString, UAnatomyUnit*> AnatomyMap;

public:	
	// Sets default values for this component's properties
	UHitHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	
};
