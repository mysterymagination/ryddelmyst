#include "BattleStats.h"
#include "BattleStatsBearer.h"

UBattleStats::UBattleStats()
{
    StatsMap = {
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
        {"MP", 100.f}
    };
}

void UBattleStats::ScaleStats(float ScaleFactor, AActor* BattleStatsBearer)
{
    StatsMap["Attack"] *= ScaleFactor;
    StatsMap["Defense"] *= ScaleFactor;
    StatsMap["Speed"] *= ScaleFactor;
    StatsMap["Magic"] *= ScaleFactor;
    StatsMap["Spirit"] *= ScaleFactor;
    IBattleStatsBearer::Execute_UpdateSpeed(BattleStatsBearer);
}