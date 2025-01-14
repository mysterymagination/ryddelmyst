// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

/**
 * Utility class for common Particle FX use cases
 */
class RYDDELMYST_API ParticleUtils
{
public:
	static void SpawnParticlesAtLocation(UObject* World, const FVector& Location, const FName& ParticlesAsset);
	static void SpawnParticlesAtLocation(UObject* World, const FVector& Location, UNiagaraSystem* ParticleSystem);
};
