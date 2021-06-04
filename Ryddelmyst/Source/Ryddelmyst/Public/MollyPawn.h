// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MollyPawnMovementComponent.h"
#include "MollyPawn.generated.h"

UCLASS()
class RYDDELMYST_API AMollyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMollyPawn();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/**
	 * Returns our special MollyPawnMovementComponent
	 */ 
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	//Input functions
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void StartGrowing();
	void StopGrowing();
	void ParticleToggle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	//Input variables
	bool bGrowing;

private:
	UPROPERTY()
	class USpringArmComponent* SpringArm;
	UPROPERTY()
	class UParticleSystemComponent* MollyParticles;
	UPROPERTY()
	class UMollyPawnMovementComponent* MollyMovementComponent;
	/**
	 * Length of the SpringArm mount we use for the camera to hang it behind our Molly 
	 */
	UPROPERTY(EditAnywhere)
	float FCameraArmLength = 800.0f;

public:
	/**
	 * Amount of time in seconds that player is allowed in which to trigger the special grow action after starting a move axis input
	 */ 
	constexpr static const float kSPECIAL_MOVE_THRESHOLD = 0.2f;
};
