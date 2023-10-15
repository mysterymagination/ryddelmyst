// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Util functions to convert and display data e.g. display a numeric value as a particle effect
 */
class RYDDELMYST_API DataViz
{
public:
	/**
	 * Generates a particle effect using the P_FloatingNumbersFX Niagara system at the given location and representing the given value
	 * @param World the world context object we're spawning the particles into
	 * @param Location a 3D vector showing where the particle FX should appear in 3D space
	 * @param Value a float that will be rounded to the nearest integer and displayed as a particle FX
	 */
	static void FX_NumberParticles(UObject* World, const FVector& Location, const FRotator& Rotation, const FVector& Scale, float Value);
};
