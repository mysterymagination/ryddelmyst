// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineGuideComponent.h"

void USplineGuideComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USplineGuideComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// todo: spawn bullets from World on a timer
	// todo: tack the towingpoint onto the bullet, rotated to match rotation of forward vector and use that point to find the nearest point to it on the spline.
}