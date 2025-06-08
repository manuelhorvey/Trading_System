#include "StructureUtils.h"
#include <OrderBlock.h>

std::vector<StructureEvent> StructureUtils::gatherStructureEvents(
    const std::vector<StructurePoint> &bosPoints,
    const std::vector<StructurePoint> &chochPoints,
    const std::vector<StructurePoint> &trendBreaks,
    const std::vector<Candle> &candles)
{
    std::vector<StructureEvent> structureEvents;

    for (const auto &bos : bosPoints)
    {
        if (bos.type != StructureType::BOS) continue;
        if (bos.index < candles.size())
        {
            const Candle &candle = candles[bos.index];
            bool isBullish = candle.close > candle.open;
            structureEvents.emplace_back(isBullish ? StructureEventType::BOS_Bullish : StructureEventType::BOS_Bearish, bos.date, bos.price);
        }
    }

    for (const auto &choch : chochPoints)
    {
        if (choch.type != StructureType::CHoCH) continue;
        if (choch.index < candles.size())
        {
            const Candle &candle = candles[choch.index];
            bool isBullish = candle.close > candle.open;
            structureEvents.emplace_back(isBullish ? StructureEventType::CHoCH_Bullish : StructureEventType::CHoCH_Bearish, choch.date, choch.price);
        }
    }

    for (const auto &tb : trendBreaks)
    {
        structureEvents.emplace_back(StructureEventType::TrendlineBreak, tb.date, tb.price);
    }

    return structureEvents;
}
