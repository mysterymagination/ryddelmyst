// Fill out your copyright notice in the Description page of Project Settings.

#include "LookitYouMovementComponent.h"
#include <algorithm>

void ULookitYouMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Make sure that everything is still valid, and that we are allowed to move.
	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	// Get (and then clear) the movement vector
	FVector InputVector = ConsumeInputVector().GetClampedToMaxSize(1.0f);
	// init velocity of 400 units/second
	FVector DesiredMovementThisFrame = InputVector * DeltaTime * 400.0f;
	if (OrbitScale)
	{
		UE_LOG(LogTemp, Warning, TEXT("LookitYouMovementComponent::TickComponent; orbitting by %f degrees %s"), DeltaTime * 20.0f, OrbitScale > 0 ? TEXT("CW") : TEXT("CCW"));
		FRotator OrbitRotation(0.f, OrbitScale * DeltaTime * 20.0f, 0.f);
		FVector PreOrbitLocation = UpdatedComponent->GetRelativeLocation();
		FVector OrbittedLocation = OrbitRotation.RotateVector(PreOrbitLocation);
		// apply orbit endpoint translations to other translations
		DesiredMovementThisFrame += OrbittedLocation - PreOrbitLocation;
	}
	if (!DesiredMovementThisFrame.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("LookitYouMovementComponent::TickComponent; after deltaT of %f, unclamped non-zero desired movement vector is %s"), DeltaTime, *DesiredMovementThisFrame.ToString());
		// check to see if we're trying to move out of flyabout bounds and clamp
		FVector AttachRelativePos = UpdatedComponent->GetRelativeLocation();
		float MaxAttachOffsetComponent = std::max(std::max(abs(AttachRelativePos.X), abs(AttachRelativePos.Y)), abs(AttachRelativePos.Z));
		if (MaxAttachOffsetComponent > MaxAllowedWandrance)
		{
			MaxAllowedWandrance = MaxAttachOffsetComponent;
		}
		FVector PossibleWandrance = AttachRelativePos + DesiredMovementThisFrame;
		UE_LOG(LogTemp, Warning, TEXT("LookitYouMovementComponent::TickComponent; lookit rel pos is %s and desired movement vector is %s so possible wandrance of lookit from attach character is %s"), *UpdatedComponent->GetRelativeLocation().ToString(), *DesiredMovementThisFrame.ToString(), *PossibleWandrance.ToString());
		if (abs(PossibleWandrance.X) >= MaxAllowedWandrance)
		{
			float OverWandrance = abs(PossibleWandrance.X) - MaxAllowedWandrance;
			if (DesiredMovementThisFrame.X >= 0)
			{
				DesiredMovementThisFrame.X -= OverWandrance;
			}
			else
			{
				DesiredMovementThisFrame.X += OverWandrance;
			}
		}
		if (abs(PossibleWandrance.Y) >= MaxAllowedWandrance)
		{
			float OverWandrance = abs(PossibleWandrance.Y) - MaxAllowedWandrance;
			if (DesiredMovementThisFrame.Y >= 0)
			{
				DesiredMovementThisFrame.Y -= OverWandrance;
			}
			else
			{
				DesiredMovementThisFrame.Y += OverWandrance;
			}
		}
		if (abs(PossibleWandrance.Z) >= MaxAllowedWandrance)
		{
			float OverWandrance = abs(PossibleWandrance.Z) - MaxAllowedWandrance;
			if (DesiredMovementThisFrame.Z >= 0)
			{
				DesiredMovementThisFrame.Z -= OverWandrance;
			}
			else
			{
				DesiredMovementThisFrame.Z += OverWandrance;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("LookitYouMovementComponent::TickComponent; after deltaT of %f, clamped desired movement vector is %s"), DeltaTime, *DesiredMovementThisFrame.ToString());

		FHitResult Hit;
		SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}
}

void ULookitYouMovementComponent::Orbit(float AxisValue)
{
	OrbitScale = AxisValue;
}
