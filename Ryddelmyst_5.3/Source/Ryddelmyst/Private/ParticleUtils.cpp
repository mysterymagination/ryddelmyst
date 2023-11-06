// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticleUtils.h"

void ParticleUtils::SpawnParticlesAtLocation(UObject* World, const FVector& Location, const FName& ParticlesAsset)
{
    UNiagaraSystem* System = LoadObject<UNiagaraSystem>
        (
            nullptr,
            *ParticlesAsset.ToString(),
            nullptr,
            LOAD_None,
            nullptr
        );
    UNiagaraComponent* FXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation
    (
        World,
        System,
        Location,
        FRotator(0.f),
        FVector(1.f, 1.f, 1.f),
        true,
        true,
        ENCPoolMethod::None,
        true
    );
}

void ParticleUtils::SpawnParticlesAtLocation(UObject* World, const FVector& Location, UNiagaraSystem* ParticleSystem)
{
    UNiagaraComponent* FloatingNumbersFX_Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation
    (
        World,
        ParticleSystem,
        Location,
        FRotator(0.f),
        FVector(1.f, 1.f, 1.f),
        true,
        true,
        ENCPoolMethod::None,
        true
    );
}
