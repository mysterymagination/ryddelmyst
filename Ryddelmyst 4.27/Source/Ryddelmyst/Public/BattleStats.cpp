#include "BattleStats.h"
#include "BattleStatsBearer.h"

void UBattleStats::ScaleStats(float ScaleFactor, AActor* BattleStatsBearer)
{
    Attack *= ScaleFactor;
    Defense *= ScaleFactor;
    Speed *= ScaleFactor;
    Magic *= ScaleFactor;
    Spirit *= ScaleFactor;
    IBattleStatsBearer::Execute_UpdateSpeed(BattleStatsBearer);
}