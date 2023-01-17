// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"
#include "FireDamageType.h"
#include "IceDamageType.h"
#include "LightningDamageType.h"

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

float UArmor::CalculateDamageRx_Implementation(AActor* BattleStatsBearer, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes) 
{
    UBattleStats* Stats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
    return BaseDamage * (1.f - GetDamageReductionFactorForDamageTypes(DamageTypes)) * AnatomyCovered->DamageScaleFactor - Stats->StatsMap["Defense"];
}

float UArmor::GetDamageReductionFactorForDamageTypes_Implementation(TArray<TSubclassOf<UDamageType>> InputDamageTypes)
{
    // todo: need a mapping of damage reduction factors to categorical damage types e.g. 
    //  fire and ice are magic and slashing is physical.  Also need to know what percentage of the damage is coming 
    //  from a given damage type, so we'll need a TMap of damage types to weight floats rather than just a TArray.  Have fun with that refactor XD
}