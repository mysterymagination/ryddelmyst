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
	/**
	 * The max allowed lifetime of the micrometeor, in seconds 
	 */
	UPROPERTY(EditAnywhere)
	float fMaxLifeTime = 10.0f;
	/**
	 * The current time this micrometeor has been alive, in seconds
	 */
	float fLifeTimer = 0.0f;
	/**
	 * The max allowed time the micrometeor lives after launching, in seconds
	 */
	UPROPERTY(EditAnywhere)
	float fMaxLaunchedLifeTime = 3.0f;
	/**
	 * The current time this micrometeor has been alive since launching, in seconds
	 */
	float fLaunchedLifeTimer = 0.0f;
	/**
	 * True if the meteor is in launched state, false otherwise
	 */
	bool bLaunched = false;
};
