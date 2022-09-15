// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitBoxComponent.generated.h"

/**
 * An Actor consisting of and managing one or more hitbox/hurtbox UShapeComponents; the specific collision handling logic is expected to be implemented in subclasses, 
 * who will also compose a specific shape e.g. UBoxComponent whose hit events will be handled by a subclass-specific (i.e. BP_ReaperTreant_Arm_Hitbox's OnHit function) handler function.
 */
UCLASS(BlueprintType, Blueprintable)
class RYDDELMYST_API AHitBoxComponent : public AActor
{
	GENERATED_BODY()
	
	
	
	
};
