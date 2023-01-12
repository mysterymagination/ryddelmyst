// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"

float UArmor::CalculateDamageRx_Implementation(AActor* BattleStatsBearer, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes) 
{
    UBattleStats* Stats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
    return BaseDamage * DamageReductionFactor * AnatomyCovered->DamageScaleFactor - Stats->StatsMap["Defense"];
}