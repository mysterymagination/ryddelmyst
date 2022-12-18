// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtremityUnit.h"
#include "BattleStats.h"

void UExtremityUnit::Debilitate_Implementation(AActor* BattleStatsBearer)
{
	// todo: need to set a clock to restore it after debilitate period expires
	if (!IsDebilitated)
	{
		if (BattleStatsBearer->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
		{
			UBattleStats* BattleStats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
			if (IsUsedForAttack)
			{
				BattleStats->ModifyStatByAttribution("anatomyunit_debilitate_" + Name, "Attack", DebilitateScaleFactor, DebilitatePeriod, BattleStatsBearer);
			}
			if (IsUsedForLocomotion)
			{
				BattleStats->ModifyStatByAttribution("anatomyunit_debilitate_" + Name, "Speed", DebilitateScaleFactor, DebilitatePeriod, BattleStatsBearer);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Debilitate; the input AActor %s does not implement IBattleStatsBearer so we cannot modify stats"), *BattleStatsBearer->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Debilitate; AActor %s is already suffering stat drop from a wound in %s!  Aim someplace else, yeesh."), *BattleStatsBearer->GetName(), *Name);
	}
}


