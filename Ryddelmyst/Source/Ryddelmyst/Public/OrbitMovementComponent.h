// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "OrbitMovementComponent.generated.h"

/**
 * A UMovementComponent derivative that attaches to the transform Component of an orbitted body and bubbles up its location modified by the desired offset vector rotated at the desired orbit speed 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API UOrbitMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrbitMovementComponent();
	///void setOrbitedBody(UPrimitiveComponent* body);
	void setOrbitedBody(AActor* body);
	///UPrimitiveComponent* getOrbitedBody();
	AActor* getOrbitedBody();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Configurable in the Editor, this UPrimitiveComponent's origin is the point on which
	 * the orbit motion will center.  Its geometry is used to help inform offset of the orbiting body.
	 */
	UPROPERTY(EditAnywhere)
	class AActor* OrbitedBody; // todo: having OrbitedBody be an AActor* does let me select MollyPawn in the editor, but ideally we'd be able to have its datatype be UPrimitiveComponent* and be able to select an actor's RootComponent that happens to be a primitivecomponent in the editor... not sure that's possible
	///class UPrimitiveComponent* OrbitedBody;
	/**
	 * The offset of the meteor's orbit from the orbitted body
	 * todo: init orbit offset based on orbited body size
	 */
	UPROPERTY(EditAnywhere)
	FVector	OrbitOffset = { 75.0f, 75.0f, 0.0f };
	/**
	 * Dictates whether or not the orbiting body should float up and down along Z in addition to orbit motion
	 */
	UPROPERTY(EditAnywhere)
	bool IsFloating = false;
	/**
	 * Dictates the speed at which the orbiting body completes a cycle of floating up and down along Z; since the floating range is derived from a sin wave over the current run time, this speed also determines the range the floating body will cover
	 */
	UPROPERTY(EditAnywhere)
	float FloatingSpeed = 0.0f;
	/**
	 * Dictates whether or not the orbiting body should rotate around its local Z axis
	 */
	UPROPERTY(EditAnywhere)
	bool IsSpinning = false;
	/**
	 * Dictates the speed at which the orbiting body rotates around local Z
	 */
	UPROPERTY(EditAnywhere)
	float SpinningSpeed = 0.0f;
	// todo: local rotation over other axes?
};
