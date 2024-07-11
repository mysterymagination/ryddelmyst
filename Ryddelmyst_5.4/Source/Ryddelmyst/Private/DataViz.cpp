// Fill out your copyright notice in the Description page of Project Settings.


#include "DataViz.h"
#include "NiagaraCommon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraDataInterfaceTexture.h"
#include "NiagaraDataInterfaceColorCurve.h"
#include "Curves/CurveLinearColor.h"
#include "Curves/RichCurve.h"
#include "MathUtils.h"

void DataViz::FX_NumberParticles(UObject* World, const FVector& Location, const FRotator& Rotation, const FVector& Scale, float Value, float DamageAmp, bool IsCrit)
{
    // round the input Value to nearest int
    int32 DamageValue = FGenericPlatformMath::RoundToInt(Value);
    // convert integer value to string
    FString DamageString = FString::FromInt(DamageValue);
    // loop through the string characters 
    float OffsetAccumulator = 0.f;
    // define the random velocity for the entire set of floating number FX systems that are rendering the whole number
    FVector RandomVelocity
    (
        MathUtils::RandomInRange(-350,350),
        MathUtils::RandomInRange(-350,350),
        MathUtils::RandomInRange(-350,350)
    );
    for (auto DamageCharacter : DamageString)
    {
        // spawn particle system at the given location plus some offset for each digit
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
        DigitTexturePath += DigitTextureFileName;
        
        UTexture* DigitTexture = LoadObject<UTexture>
        (
            nullptr,
            *DigitTexturePath,
            nullptr,
            LOAD_None,
            nullptr
        );
        // todo: pretty sure we're not loading this correctly and are just using the default curve we set in the particle system
        FString ColorCurvePath = TEXT("/Game/Ryddelmyst_Assets/Textures/");
        if (IsCrit)
        {
            ColorCurvePath += TEXT("RainbowRoadColorCurve");
        }
        else
        {
            ColorCurvePath += TEXT("WhiteColorCurve");
        }
        UCurveLinearColor* ColorCurveAsset = LoadObject<UCurveLinearColor>
        (
            nullptr,
            *ColorCurvePath,
            nullptr,
            LOAD_None,
            nullptr
        );
        UE_LOG(LogTemp, Warning, TEXT("FX_NumberParticles; DigitTexturePath is %s and texture object says %p and color curve asset path is %s and color curve asset object says %p"), *DigitTexturePath, DigitTexture, *ColorCurvePath, ColorCurveAsset);
        UNiagaraComponent* FloatingNumbersFX_Component = UNiagaraFunctionLibrary::SpawnSystemAtLocation
        (
            World,
            FloatingNumbersFX_System,
            Location,
            FRotator(180.f, UGameplayStatics::GetPlayerController(World, 0)->GetControlRotation().Yaw, 0.f),
            FVector(1.f,1.f,1.f),
            true,
            true,
            ENCPoolMethod::None,
            true
        );
        FloatingNumbersFX_Component->AddLocalRotation(Rotation);
        FloatingNumbersFX_Component->AddLocalOffset(FVector(0.f, OffsetAccumulator, 0.f), false, nullptr, ETeleportType::None);
        FloatingNumbersFX_Component->SetNiagaraVariableFloat(TEXT("DamageAmp"), DamageAmp);
        FloatingNumbersFX_Component->SetNiagaraVariableBool(TEXT("IsCrit"), IsCrit);
        FloatingNumbersFX_Component->SetNiagaraVariableVec3(TEXT("RandomVelocity"), RandomVelocity);
        UNiagaraDataInterface* DataInterfaceForDigitTexture = UNiagaraFunctionLibrary::GetDataInterface(UNiagaraDataInterface::StaticClass(), FloatingNumbersFX_Component, FName{ TEXT("DigitTexture") });
        if (DataInterfaceForDigitTexture)
        {
            if (const auto DataInterfaceTextureSample = Cast<UNiagaraDataInterfaceTexture>(DataInterfaceForDigitTexture))
            {
                DataInterfaceTextureSample->SetTexture(DigitTexture);
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("FX_NumberParticles; failed to cast to DataInterfaceTexture"))
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FX_NumberParticles; failed to get Niagara data interface to DigitTexture"));
        }
        UNiagaraDataInterface* DataInterfaceForDamageColorCurve = UNiagaraFunctionLibrary::GetDataInterface(UNiagaraDataInterface::StaticClass(), FloatingNumbersFX_Component, FName{ TEXT("DamageColorCurve") });
        if (DataInterfaceForDamageColorCurve)
        {
            if (const auto DataInterfaceColorCurve = Cast<UNiagaraDataInterfaceColorCurve>(DataInterfaceForDamageColorCurve))
            {
                // todo: assuming we are loading the color curve asset correctly, then this is presumably not the correct way to set the system's param from it since we see the default curve value being used at runtime. See https://forums.unrealengine.com/t/how-can-i-set-a-uniagaradatainterfacecolorcurve-from-a-color-curve-asset/1343769 for more.
                UE_LOG(LogTemp, Warning, TEXT("FX_NumberParticles; setting RGBA curves on the data interface from the loaded asset... red curve first key value is %f at time %f, green curve first key value is %f at time %f, and blue curve first key value is %f at time %f."), 
                    ColorCurveAsset->FloatCurves[0].Keys[0].Value, 
                    ColorCurveAsset->FloatCurves[0].Keys[0].Time,
                    ColorCurveAsset->FloatCurves[1].Keys[0].Value,
                    ColorCurveAsset->FloatCurves[1].Keys[0].Time,
                    ColorCurveAsset->FloatCurves[2].Keys[0].Value,
                    ColorCurveAsset->FloatCurves[2].Keys[0].Time);
                DataInterfaceColorCurve->RedCurve = ColorCurveAsset->FloatCurves[0];
                DataInterfaceColorCurve->GreenCurve = ColorCurveAsset->FloatCurves[1];
                DataInterfaceColorCurve->BlueCurve = ColorCurveAsset->FloatCurves[2];
                DataInterfaceColorCurve->AlphaCurve = ColorCurveAsset->FloatCurves[3];
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("FX_NumberParticles; failed to cast to DataInterfaceColorCurve"))
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("FX_NumberParticles; failed to get Niagara data interface to DamageColorCurve"));
        }
        OffsetAccumulator += 50.f * DamageAmp;
    }
}
