// Fill out your copyright notice in the Description page of Project Settings.


#include "DataViz.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void DataViz::FX_NumberParticles(UObject* World, const FVector& Location, const FRotator& Rotation, const FVector& Scale, float Value)
{
    // todo: round the input Value to nearest int
    // todo: convert integer value to string
    // todo: loop through the string characters 
    // todo: spawn particle system at the given location plus some offset for each
    //  e.g. '1' '2' '3' for input 123.1
    UNiagaraSystem* FloatingNumbersFX_System = LoadObject<UNiagaraSystem>
    (
        nullptr, 
        TEXT("/Game/Ryddelmyst_Assets/Particles/P_FloatingNumbersFX.P_FloatingNumbersFX"), 
        nullptr, 
        LOAD_None, 
        nullptr
    );
    // todo: how do we provide inputs to the niagara system?
    UNiagaraComponent* FloatingNumbersFX_Component = SpawnSystemAtLocation
    (
        World,
        FloatingNumbersFX_System,
        Location,
        Rotation,
        true,
        true,
        ENCPoolMethod::None,
        true
    );
}
