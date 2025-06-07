#pragma once
#include <vector>
#include "Candle.h"
#include "MarketStructure.h"
#include <OrderBlock.h>

namespace StructureUtils
{
    std::vector<StructureEvent> gatherStructureEvents(
        const std::vector<StructurePoint> &bosPoints,
        const std::vector<StructurePoint> &chochPoints,
        const std::vector<StructurePoint> &trendBreaks,
        const std::vector<Candle> &candles);
}
