// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MicroMeteorComponent.generated.h"

/**
 * A smol sphere that orbits its attach parent and zooms off on an orthogonal vector from it (while on fire) when its time is up
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API UMicroMeteorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMicroMeteorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Launches this meteor off on a vector orthogonal to the orbitted body at the time of launch, then destroys it when the launch party is over.
	 */
	void MeteoricLaunch();

private:
	UPROPERTY()
	class UParticleSystemComponent* MeteorParticles;
	/**
	 * The offset of the meteor's orbit from the orbitted body 
	 */
	UPROPERTY(EditAnywhere)
	FVector	OrbitOffset = {0.0f, 0.0f, 0.0f};
	/**
	 * The offset of the meteor from its world position at the time of launch
	 * TODO: could probably get by using only the OrbitOffset and SetRelativeLocation() for the launch ortho path if we invert the rotation transform of the parent component out of the final transform applied to the child meteor
	 */
	FVector	LaunchedOffset = { 0.0f, 0.0f, 0.0f };
	/**
	 * The max allowed lifetime of the micrometeor, in seconds 
	 */
	UPROPERTY(EditAnywhere)
	float fMaxLifeTime = 5.0f;
	/**
	 * The current time this micrometeor has been alive, in seconds
	 */
	float fLifeTimer = 0.0f;
	/**
	 * The max allowed time the micrometeor lives after launching, in seconds
	 */
	UPROPERTY(EditAnywhere)
	float fMaxLaunchedLifeTime = 5.0f;
	/**
	 * The current time this micrometeor has been alive since launching, in seconds
	 */
	float fLaunchedLifeTimer = 0.0f;
	/**
	 * True if the meteor is in launched state, false otherwise
	 */
	bool bLaunched = false;
	/**
	 * Accumulates Yaw of the parent torch's orbit so that we can remove it and continue a steady launch vector relative to the position the orbitted
	 * torch was in when we entered launched state
	 */ 
	FRotator OrbitAccumulator;
};
