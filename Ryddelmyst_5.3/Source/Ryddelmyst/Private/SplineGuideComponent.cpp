// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineGuideComponent.h"

void USplineGuideComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!Spline)
	{
		// todo: populate spline parametrically. Maybe make a few enum categories for pattern like sine wave and then scalers or something so the wave amplitude and period change.
		// todo: as a first step, let's go with sine wave and/or y=x^2 
		int SplinePointCount = 10;
		for (int PointIdx = 0; PointIdx < SplinePointCount; PointIdx++)
		{

		}
		Spline->AddPoint();
	}
	// todo: add more splines, maybe three or five total in a one of couple spread patterns out from the host Actor. Ideal thing might be to create a cone with bullet spray range for height out from the host Actor and pick 3-5 vectors randomly inside that cone. 
}

void USplineGuideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// todo: spawn bullet from World.
	// todo: tack the towingpoint onto the bullet, rotated to match rotation of forward vector and use that point to find the nearest point to it on the spline.
	// todo: create a timer to spawn additional bullets up to the amount we want on a given spline.
}