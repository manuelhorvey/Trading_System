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
        bool isBullish = bos.index > 0 && bos.index < candles.size() && bos.price > candles[bos.index - 1].close;
        structureEvents.emplace_back(isBullish ? StructureEventType::BOS_Bullish : StructureEventType::BOS_Bearish, bos.date, bos.price);
    }

    for (const auto &choch : chochPoints)
    {
        if (choch.type != StructureType::CHoCH) continue;
        bool isBullish = choch.index > 0 && choch.index < candles.size() && choch.price > candles[choch.index - 1].close;
        structureEvents.emplace_back(isBullish ? StructureEventType::CHoCH_Bullish : StructureEventType::CHoCH_Bearish, choch.date, choch.price);
    }

    for (const auto &tb : trendBreaks)
    {
        structureEvents.emplace_back(StructureEventType::TrendlineBreak, tb.date, tb.price);
    }

    return structureEvents;
}
