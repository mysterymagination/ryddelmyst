#include "BattleStats.h"
#include "BattleStatsBearer.h"

void BattleStats::ScaleStats(float ScaleFactor, AActor* BattleStatsBearer);
{
    Attack *= ScaleFactor;
    Defense *= ScaleFactor;
    Speed *= ScaleFactor;
    Magic *= ScaleFactor;
    Spirit *= ScaleFactor;
    IBattleStatsBearer::Execute_UpdateSpeed(BattleStatsBearer);
}