// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
	virtual void EnableCamera(bool enable) override;
	virtual void TakeControl() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:	
	// State variables
	bool FollowMode = true;
	bool CameraActive = false;

private:
	UPROPERTY()
	class USpringArmComponent* SpringArm;
	UPROPERTY()
	class UCameraComponent* Camera;
	UPROPERTY()
	class ULookitYouMovementComponent* Movement;
	/**
	 * Length of the SpringArm mount we use for the camera to hang it behind our Molly 
	 */
	UPROPERTY(EditAnywhere)
	float FCameraArmLength = 400.0f;
	/**
	 * The Pawn that this LookitYouPawn should be following when not in flyabout mode
	 */ 
	UPROPERTY(EditAnywhere)
	class APawn* FollowPawn;
};
