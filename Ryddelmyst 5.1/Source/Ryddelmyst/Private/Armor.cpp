// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"

float UArmor::CalculateDamageRx_Implementation(AActor* BattleStatsBearer, UAnatomyUnit* AnatomyCovered, float BaseDamage, const TArray<TSubclassOf<UDamageType>>& DamageTypes) 
{
    UBattleStats* Stats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
    // todo: need a mapping of damage reduction factors to categorical damage types e.g. fire and ice are magic and slashing is physical.  Also need to know what percentage of the damage is coming from a given damage type, so we'll need a TMap of damage types to weight floats rather than just a TArray.  Have fun with that refactor XD
    return BaseDamage * GetDamageReductionFactorForDamageTypes(DamageTypes) * AnatomyCovered->DamageScaleFactor - Stats->StatsMap["Defense"];
}