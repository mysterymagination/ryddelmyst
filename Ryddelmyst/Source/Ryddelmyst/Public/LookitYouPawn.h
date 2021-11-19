// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LookitYou.h"
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
	void Turn(float AxisValue);

	// ILookitYou impl
	void EnableCamera(bool enable);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	//Input variables
	bool bFollowMode;

private:
	UPROPERTY()
	class USpringArmComponent* SpringArm;
	UPROPERTY()
	class UCameraComponent* Camera;
	/**
	 * Length of the SpringArm mount we use for the camera to hang it behind our Molly 
	 */
	UPROPERTY(EditAnywhere)
	float FCameraArmLength = 800.0f;
};
