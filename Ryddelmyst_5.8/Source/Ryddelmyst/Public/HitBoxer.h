// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitBoxer.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitBoxer : public UInterface
{
	GENERATED_BODY()
};

/**
 * A creature or object that has hitboxes/hurtboxes and needs to process OnComponentHitEvents in a common way 
 */
class RYDDELMYST_API IHitBoxer
{
	GENERATED_BODY()

public:
	/**
 	 * Handles a OnComponentHitEvent assuming a hit paradigm of striker-first. This means that when OnHit is called,
	 * the implementor is the one striking something as opposed to being stricken; if the implementor's strikingcomponent
	 * implements IAttacker we might e.g. forward the arguments on to a UWeapon's UAttack::OnHit() etc.
	 * The UDefender side of the equation for the strickenactor/component is handled internally once the attack flow has kicked off
	 * so implementors need not concern themselves with their own defender role -- the other Actor in the collision will fire off its
	 * own OnComponentHitEvent with itself in the role of striker to address that matter via its own attack flow.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Combat")
	void OnHit(UPrimitiveComponent* StrikingComp, AActor* StrickenActor, UPrimitiveComponent* StrickenComp, FVector NormalImpulse, const FHitResult& HitInfo);
};
