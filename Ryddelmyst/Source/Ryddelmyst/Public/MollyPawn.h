// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);
	void StartGrowing();
	void StopGrowing();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	//Input variables
	FVector CurrentVelocity;
	bool bGrowing;
	/**
	 * Aggregates deltatime from incoming frames wherein we've continuously held down the MoveX or MoveY inputs
	 */  
	float fMoveTime;

public:
	UPROPERTY(EditAnywhere)
	USceneComponent* VisibleMollyComponent;
	/**
	 * Amount of time in seconds that player is allowed in which to trigger the special grow action after starting a move axis input
	 */ 
	constexpr static const float kSPECIAL_MOVE_THRESHOLD = 0.3f;
};
