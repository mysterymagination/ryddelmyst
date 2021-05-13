// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrbitMovementComponent.h"
#include "MicroMeteor.generated.h"

/**
 * A smol sphere that orbits its attach parent and zooms off on an orthogonal vector from it (while on fire) when its time is up
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API AMicroMeteor : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AMicroMeteor();
	size_t getId();
	void setId(size_t id);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/**
	 * Launches this meteor off on a vector orthogonal to the orbitted body at the time of launch, then destroys it when the launch party is over.
	 */
	void MeteoricLaunch();

private:
	/**
	 * ID used to track the meteor, assigned by its orbitted body when it's first attached thereto 
	 */
	size_t mId = 0;
	UPROPERTY()
	class UParticleSystemComponent* MeteorParticles;
	/**
	 * Vector of magnitude 1 along which we will travel once launched
	 */
	FVector	LaunchVector = { 0.0f, 0.0f, 0.0f };
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
	 * The speed at which the launched meteor flies, in map units/second
	 */
	UPROPERTY(EditAnywhere)
	float fLaunchSpeed = 2000.0f;
	/**
	 * Controls the meteor's orbit motion around an attached body
	 */
	UPROPERTY()
	class UOrbitMovementComponent* OrbitMotion;
};
