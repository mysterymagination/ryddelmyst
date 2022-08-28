// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtremityUnit.h"
#include "BattleStats.h"

void UExtremityUnit::Debilitate(AActor* BattleStatsBearer)
{
	// todo: need to set a clock to restore it after debilitate period expires
	if (!IsDebilitated)
	{
		if (BattleStatsBearer->GetClass()->ImplementsInterface(UBattleStatsBearer::StaticClass()))
		{
			UBattleStats* BattleStats = IBattleStatsBearer::Execute_GetStats(BattleStatsBearer);
			if (IsUsedForAttack)
			{
				BattleStats->StatsMap["Attack"] *= DebilitateScaleFactor;
				BattleStats->StatMods["anatomyunit_debilitate_" + std::string(TCHAR_TO_UTF8(*GetName())) + "_" + std::string(TCHAR_TO_UTF8(*FormName))].push_back({std::string("Attack"), DebilitateScaleFactor});
			}
			if (IsUsedForLocomotion)
			{
				BattleStats->StatsMap["Speed"] *= DebilitateScaleFactor;
				IBattleStatsBearer::Execute_UpdateSpeed(BattleStatsBearer);
				BattleStats->StatMods["anatomyunit_debilitate_" + std::string(TCHAR_TO_UTF8(*GetName())) + "_" + std::string(TCHAR_TO_UTF8(*FormName))].push_back({ std::string("Speed"), DebilitateScaleFactor });
			}
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, FName("OnEffectRemoved"), EffectedActor);
			EffectedActor->GetWorldTimerManager().SetTimer(ShockTimerHandle, TimerDelegate, ShockDuration, false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Debilitate; the input AActor %s does not implement IBattleStatsBearer so we cannot modify stats"), *BattleStatsBearer->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Debilitate; AActor %s is already suffering stat drop from a wound in %s!  Aim someplace else, yeesh."), *BattleStatsBearer->GetName(), *FormName);
	}
}


