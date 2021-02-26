// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MicroMeteorComponent.generated.h"

/**
 * A smol sphere that orbits its attach parent
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
	 * The max allowed lifetime of the micrometeor, in seconds 
	 */
	UPROPERTY(EditAnywhere)
	float fMaxLifeTime = 10.0f;
	/**
	 * The current time this micrometeor has been alive, in seconds
	 */
	float fLifeTime = 0.0f;
};
