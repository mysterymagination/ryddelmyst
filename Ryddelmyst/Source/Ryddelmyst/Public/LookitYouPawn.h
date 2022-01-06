// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LookitYou.h"
#include "LookitYouMovementComponent.h"
#include "LookitYouPawn.generated.h"

UCLASS()
class RYDDELMYST_API ALookitYouPawn : public APawn, public ILookitYou
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALookitYouPawn();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input functions
	void FlyAbout();
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveUp(float AxisValue);
	void Orbit(float AxisValue);

	// field accessors
	void SetFollowPawn(APawn* followPawn);
	APawn* GetFollowPawn();

	// ILookitYou impl
	virtual void TakeControl() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UCameraComponent* Camera;
	UPROPERTY()
	class ULookitYouMovementComponent* Movement;
	/**
	 * The Pawn that this LookitYouPawn should be following when not in flyabout mode
	 */ 
	UPROPERTY(EditAnywhere)
	APawn* FollowPawn;
};
