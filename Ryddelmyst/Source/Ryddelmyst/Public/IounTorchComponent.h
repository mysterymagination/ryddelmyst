// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "IounTorchComponent.generated.h"

/**
 * An IounTorch is a sphere that orbits around its USceneComponent parent, wreathed in heatless firelight
 */ 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RYDDELMYST_API UIounTorchComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIounTorchComponent();
	/**
	 * @return the primitive component subclass that defines the torch's physical form
	 */
	UPrimitiveComponent* getPhysicality();
	/**
	 * @return the accumulated yaw rotation of this torch around its orbitted body; for use by child micrometeors that need to remove this orbit from the equation once they launch 
	 */
	FRotator getOrbitAccumulator();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Interval in seconds we should wait before spawning a meteor
	 */
	float fMeteorSpawnInterval = 3.0f;
	/**
	 * Incrementing timer marking the time since component inception or last meteor spawn
	 */
	float fMeteorSpawnTimer = 0.0f;
	/**
	 * The number of meteors we have spawned
	 */
	uint8 iMeteorCount = 0;
	/**
	 * The max number of meteors we allow to spawn 
	 */
	uint8 iMaxMeteors = 1;
	/**
	 * Defines the radius of the tirch's orbit around the orbitted body 
	 */
	FVector OrbitOffset;
	/**
	 * Accumulates Yaw degrees rotated as the torch orbits a body 
	 */
	FRotator OrbitAccumulator;
	/**
	 * Root of this component's scene hierarchy, defining its basic physicality
	 */
	UPROPERTY()
	class UPrimitiveComponent* SceneRoot;
	/**
 	 * Fire particles emitted by the IounTorch
	 * TODO: emissive material!  I wanna see glowy bits 
	 */  
	UPROPERTY()
	class UParticleSystemComponent* TorchParticles;
};
