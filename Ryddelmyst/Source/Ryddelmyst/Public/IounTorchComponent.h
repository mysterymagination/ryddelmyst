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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/**
	 * Root of this component's scene hierarchy
	 */
	UPROPERTY()
	class USceneComponent* SceneRoot;
	/**
 	 * Fire particles emitted by the IounTorch
	 * TODO: emissive material!  I wanna see glowy bits 
	 */  
	UPROPERTY()
	class UParticleSystemComponent* TorchParticles;
};
