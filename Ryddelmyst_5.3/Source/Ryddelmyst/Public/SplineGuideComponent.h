// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "SplineGuideComponent.generated.h"

/**
 * Moves an array of Actor bullets' root SceneComponents along a spline path; the path may be prefabricated and provided or generated at runtime with parameters.
 * We'll have a single spline with this component directing bullets' movement along it; we'll set up a transform attachment between the spline and the spawned bullet actors and then apply AddMovementInput() with a given speed and direction derived by using a point placed in front of the bullet and sampling for the nearest spline point to it with FindLocationClosestToWorldLocation() so the bullet is continously making its way along the spline.
 */
UCLASS()
class RYDDELMYST_API USplineGuideComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	/**
	 * Pointer to the USplineComponent that this component guides actors along. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
	USplineComponent* Spline;
	
};
