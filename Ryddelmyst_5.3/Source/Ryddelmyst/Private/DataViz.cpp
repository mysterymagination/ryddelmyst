// Fill out your copyright notice in the Description page of Project Settings.


#include "DataViz.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void DataViz::FX_NumberParticles(UObject* World, const FVector& Location, const FRotator& Rotation, const FVector& Scale, float Value, float DamageAmp, bool IsCrit)
{
    // round the input Value to nearest int
    int32 DamageValue = FGenericPlatformMath::RoundToInt(Value);
    // convert integer value to string
    FString DamageString = FString::FromInt(DamageValue);
    // loop through the string characters 
    float OffsetAccumulator = 0.f;
    for (auto DamageCharacter : DamageString)
    {
        // spawn particle system at the given location plus some offset for each
        //  e.g. '1' '2' '3' for input 123.1
        UNiagaraSystem* FloatingNumbersFX_System = LoadObject<UNiagaraSystem>
        (
            nullptr,
            TEXT("/Game/Ryddelmyst_Assets/Particles/P_FloatingNumbersFX.P_FloatingNumbersFX"),
            nullptr,
            LOAD_None,
            nullptr
        );
        FString DigitTexturePath = TEXT("/Game/Ryddelmyst_Assets/Textures/Digits/Alagard/");
        FString DigitTextureFileName = TEXT("");
        switch (DamageCharacter)
        {
            case '0':
                DigitTextureFileName = TEXT("0.0");
                break;
            case '1':
                DigitTextureFileName = TEXT("1.1");
                break;
            case '2':
                DigitTextureFileName = TEXT("2.2");
                break;
            case '3':
                DigitTextureFileName = TEXT("3.3");
                break;
            case '4':
                DigitTextureFileName = TEXT("4.4");
                break;
            case '5':
                DigitTextureFileName = TEXT("5.5");
                break;
            case '6':
                DigitTextureFileName = TEXT("6.6");
                break;
            case '7':
                DigitTextureFileName = TEXT("7.7");
                break;
            case '8':
                DigitTextureFileName = TEXT("8.8");
                break;
            case '9':
                DigitTextureFileName = TEXT("9.9");
                break;
            default:
                DigitTextureFileName = TEXT("0.0");
                break;
        }
        UTexture* DigitTexture = LoadObject<UTexture>
        (
            nullptr,
            *DigitTexturePath,
            nullptr,
            LOAD_None,
            nullptr
        );
        UNiagaraComponent* FloatingNumbersFX_Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation
        (
            World,
            FloatingNumbersFX_System,
            Location,
            Rotation,
            FVector(1.f,1.f,1.f),
            true,
            true,
            ENCPoolMethod::None,
            true
        );
        FloatingNumbersFX_Component->AddLocalOffset(FVector(0.f, OffsetAccumulator, 0.f), false, nullptr, ETeleportType::None);
        FloatingNumbersFX_Component->SetNiagaraVariableFloat(TEXT("DamageAmp"), DamageAmp);
        FloatingNumbersFX_Component->SetNiagaraVariableBool(TEXT("IsCrit"), IsCrit);
        FloatingNumbersFX_Component->SetNiagaraVariableObject(TEXT("DigitTexture"), DigitTexture);
        OffsetAccumulator += 20.f;
    }
}
