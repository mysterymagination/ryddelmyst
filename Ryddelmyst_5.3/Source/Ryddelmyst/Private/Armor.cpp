// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"
#include "FireDamageType.h"
#include "IceDamageType.h"
#include "LightningDamageType.h"
#include "BludgeoningDamageType.h"
#include "SlashingDamageType.h"
#include "PiercingDamageType.h"

UArmor::UArmor()
{
    DamageCatMap = 
    {
        {
            UFireDamageType::StaticClass(), EDamageCat::Magical
        },
        {
            UIceDamageType::StaticClass(), EDamageCat::Magical
        },
        {
            ULightningDamageType::StaticClass(), EDamageCat::Magical
        },
        {
            UBludgeoningDamageType::StaticClass(), EDamageCat::Physical
        },
        {
            USlashingDamageType::StaticClass(), EDamageCat::Physical
        },
        {
            UPiercingDamageType::StaticClass(), EDamageCat::Physical
        },
    };
}

float UArmor::CalculateDamageRx_Implementation(FBattleStatsData BattleStatsData, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TMap<TSubclassOf<UDamageType>, float>& DamageTypesToWeightsMap)
{
    return BaseDamage * (1.f - GetDamageReductionFactorForDamageTypes(DamageTypesToWeightsMap)) * AnatomyCovered->DamageScaleFactor - BattleStatsData.StatsMap["Defense"];
}

float UArmor::GetDamageReductionFactorForDamageTypes_Implementation(const TMap<TSubclassOf<UDamageType>, float>& InputDamageTypesMap)
{
    float DRFactorSum = 0.f;
    for(EDamageCat DamageCat : TEnumRange<EDamageCat>())
    {
        float CatSum = 0.f;
        for(auto& DamageType : InputDamageTypesMap)
        {
            // for each damage type that matches the current DamageCat, tally the weights into a final sum percentage of damage for the current DamageCat (CatSum)
            if(*DamageCatMap.Find(DamageType.Key) == DamageCat) 
            {
                CatSum += DamageType.Value;
                UE_LOG(LogTemp, Warning, TEXT("GetDamageReductionFactorForDamageTypes; DamageType %s of DamageCat %s adds itself to the catsum for a running total %f"), 
                    *DamageType.Key->GetName(), 
                    *UEnum::GetDisplayValueAsText(DamageCat).ToString(),
                    CatSum
                );
            }
        }
        // multiply CatSum by the relevant DR factor and add the result to DRFactorSum
        float* DRptr = DamageReductionMap.Find(DamageCat);
        if(DRptr)
        {
            DRFactorSum += CatSum * (*DRptr);
            UE_LOG(LogTemp, Warning, TEXT("GetDamageReductionFactorForDamageTypes; DamageCat %s sum %f multiplied by relevant DR factor %f adds the product %f to DR factor sum for a running total %f"), 
                    *UEnum::GetDisplayValueAsText(DamageCat).ToString(),
                    CatSum,
                    *DRptr,
                    CatSum * (*DRptr),
                    DRFactorSum
                );
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("GetDamageReductionFactorForDamageTypes; returning final DR factor %f"), DRFactorSum);
    return DRFactorSum;
}