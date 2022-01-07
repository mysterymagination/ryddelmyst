// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "LookitYouMovementComponent.generated.h"

/**
 * A PawnMovementComponent directing a LookitYouPawn to fly about within a radius around its associated FollowCharacter
 */
UCLASS()
class RYDDELMYST_API ULookitYouMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/**
	 * Moves the LookitYouPawn in an orbit around its FollowCharacter, updating location per-frame to be at a Yaw rotated vector that keeps the offset magnitude constant.  
	 * @param AxisValue rotation will be CW for positive input, CCW otherwise.
	 */
	void Orbit(float AxisValue);
private:
	/**
	 * Maximum map units the LookitYouPawn is allowed to stray away from the FollowCharacter on any given axis.  Once this distance is met during a FlyAbout, the motion vector should drop its component corresponding to the axis that hit the limit to 0. 
	 * todo: this value should probably be at least 1000 map units
	 */
	UPROPERTY(EditAnywhere)
	float MaxAllowedWandrance = 500.f;
	/**
	 * Indicates how our orbit movement should proceed, CCW if scale if negative, CW if positive, and no orbit if 0. 
	 */
	float OrbitScale = 0.f;
};
