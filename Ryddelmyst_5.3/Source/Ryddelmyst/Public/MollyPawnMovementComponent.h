// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "MollyPawnMovementComponent.generated.h"

/**
 * A PawnMovementComponent directing a MollyPawn, moving her silkily along walls and so forth
 */
UCLASS()
class RYDDELMYST_API UMollyPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	float getMoveTime();
private:
	/**
	 * Aggregates deltatime from incoming frames wherein we've continuously moved
	 */  
	float fMoveTime = 0.0f;
};
