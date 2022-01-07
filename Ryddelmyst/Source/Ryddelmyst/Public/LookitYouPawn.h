// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LookitYou.h"
#include "FawnCharacter.h"
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
	void EndFlyAbout();
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveUp(float AxisValue);
	void Orbit(float AxisValue);

	// field accessors
	void SetFollowPawn(AFawnCharacter* followPawn);
	AFawnCharacter* GetFollowPawn();

	// ILookitYou impl
	// Since we're a Pawn with our own camera and we're getting spawned when TakeControl() is called, we don't need to mess with camera settings
	virtual void EnableCamera(bool enable) override;
	// Unpossess the FollowPawn and possess this Pawn instead
	virtual void TakeControl() override;
	// Return the LookitYouPawn's location in world space
	virtual FVector GetLocation() override;
	// Return the LookitYouPawn's rotation in world space
	virtual FRotator GetRotation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UCameraComponent* Camera;
	UPROPERTY()
	class ULookitYouMovementComponent* Movement;
	/**
	 * The FawnCharacter that this LookitYouPawn should will need be bound around and to which it will return possession control when the user indicates flyabout is done.
	 */ 
	UPROPERTY()
	AFawnCharacter* FollowPawn;
};
