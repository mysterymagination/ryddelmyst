#include "BattleStats.h"
#include "BattleStatsBearer.h"

UBattleStats::UBattleStats()
{
    StatsData.StatsMap = {
        /**
         * @brief Represents the characters experience level, which influences the number of dice rolled to add random damage to spells.  
         * TODO: add some stat growth curve associations with this value so level modifications up and down can affect the character overall.
         *
         */
        {"Level", 1.f},
        /**
         * @brief Determines physical damage from melee weapons
         *
         */
        {"Attack", 10.f},
        /**
         * @brief Determines the reduction in damage taken from physical sources
         *
         */
        {"Defense", 10.f},
        /**
         * @brief Determines magic damage dealt to a target before their Spirit or Defense is taken into account
         *
         */
        {"Magic", 10.f},
        /**
         * @brief Determines the reduction in damage taken from magical energy sources
         *
         */
        {"Spirit", 10.f},
        /**
         * @brief This is a scaling factor for the speed component of a moving Actor's velocity, usually controlled by a variable within its MovementComponent subclass e.g. Max*Speed for CharacterMovementComponent.
         *
         */
        {"Speed", 1.f},
        /**
         * @brief This is the character's health points; lte 0 means death.
         *
         */
        {"HP", 100.f},
        /**
         * @brief This is the character's mana points; a value gte a spell's cost is required to cast the spell.
         *
         */
        {"MP", 100.f},
        /**
         * @brief This is the character's max health points; can be used e.g. to compare against current health in key HP above to gauge health percentage.
         *
         */
        {"MaxHP", 100.f},
        /**
         * @brief This is the character's max mana points; can be used e.g. to compare against current mana in key MP above to gauge mana percentage.
         *
         */
        {"MaxMP", 100.f}
    };
}

void UBattleStats::ResetStats()
{
    StatsData.StatsMap["HP"] = StatsData.StatsMap["MaxHP"];
    StatsData.StatsMap["MP"] = StatsData.StatsMap["MaxMP"];
}

void UBattleStats::ScaleStats(float ScaleFactor, AActor* BattleStatsBearer)
{
    StatsData.StatsMap["Attack"] *= ScaleFactor;
    StatsData.StatsMap["Defense"] *= ScaleFactor;
    StatsData.StatsMap["Speed"] *= ScaleFactor;
    StatsData.StatsMap["Magic"] *= ScaleFactor;
    StatsData.StatsMap["Spirit"] *= ScaleFactor;
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, "Attack");
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, "Defense");
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, "Speed");
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, "Magic");
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, "Spirit");
}

void UBattleStats::ModifyStatByAttribution(const FString& AttributionName, const FString& StatName, float ScaleFactor, float Duration, AActor* BattleStatsBearer)
{
    UE_LOG(LogTemp, Warning, TEXT("ModifyStatByAttribution; stat %s is being scaled by %f, caused by effect %s"), *StatName, ScaleFactor, *AttributionName);
    StatsData.StatsMap[StatName] *= ScaleFactor;
    StatMods[std::string(TCHAR_TO_UTF8(*AttributionName))].push_back({ TCHAR_TO_UTF8(*StatName), ScaleFactor});
    IBattleStatsBearer::Execute_HandleStatModification(BattleStatsBearer, StatName);
    if (Duration > 0.f)
    {
        FTimerDelegate DebilitateTimerDelegate;
        DebilitateTimerDelegate.BindUFunction(this, FName("UnmodifyStatsByAttribution"), AttributionName, BattleStatsBearer);
        // todo: record this timer handle in a struct or something alongside the stat and scalefactor values mapped to attribution name?  I don't anticipate needing to lookup these clocks after we start them, but it might come in handy.  Also if the timermanager actually requires the TimerHandle reference it takes to be valid internally for any purpose...
        FTimerHandle TossawayHandle;
        BattleStatsBearer->GetWorldTimerManager().SetTimer(TossawayHandle, DebilitateTimerDelegate, Duration, false);
    }
}

void UBattleStats::UnmodifyStatsByAttribution(const FString& AttributionName, AActor* BattleStatsBearer)
{
    auto AttributionString = std::string(TCHAR_TO_UTF8(*AttributionName));
    // lookup our stat mods attributed to this effect and reverse them
    auto ModVector = StatMods[AttributionString];
    for (auto Mod : ModVector)
    {
        // the first pair element is an std::string and we want an FString, so convert
        FString StatName(Mod.first.c_str());
        UE_LOG(LogTemp, Warning, TEXT("UnmodifyStatByAttribution; stat %s is being scaled by %f, to reverse the effect %s"), *StatName, 1.f / Mod.second, *AttributionName);
        // scale the stat named by the first element of the pair by the inverse of the scaling factor given in the second element of the pair
        StatsData.StatsMap[StatName] *= 1.f/Mod.second;
    }
    // remove this attribution key from the statsmod map
    StatMods.erase(AttributionString);
}