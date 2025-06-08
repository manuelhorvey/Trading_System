#include "OrderBlockAnalyzer.h"
#include "TradingUtils.h"
#include "StructureUtils.h"
#include "LoggingUtils.h"
#include <spdlog/spdlog.h>
#include <algorithm>

OrderBlockAnalyzer::OrderBlockAnalyzer(const Config &config)
    : reader(config.csv_path, config.data_source, config.api_endpoint, config.api_key),
      lastOrderBlockDate(""),
      lastCHoCHDate("")
{
}

void OrderBlockAnalyzer::analyze()
{
    auto candles = reader.readData();

    if (candles.size() < 50)
    {
        spdlog::error("Not enough data (less than 50 bars available).");
        return;
    }

    // Log latest candle with volume delta instead of structure update
    const Candle &latestCandle = candles.back();
    const Candle *prevCandle = (candles.size() > 1) ? &candles[candles.size() - 2] : nullptr;

    spdlog::info(" Latest Candle:");
    if (prevCandle)
    {
        double deltaVol = latestCandle.volume - prevCandle->volume;
        spdlog::info(" Date: {}, O: {:.2f}, H: {:.2f}, L: {:.2f}, C: {:.2f}, Vol: {}, ΔVol: {}",
                     latestCandle.date, latestCandle.open, latestCandle.high, latestCandle.low, latestCandle.close,
                     latestCandle.volume, deltaVol);
    }
    else
    {
        spdlog::info(" Date: {}, O: {:.2f}, H: {:.2f}, L: {:.2f}, C: {:.2f}, Vol: {}",
                     latestCandle.date, latestCandle.open, latestCandle.high, latestCandle.low, latestCandle.close,
                     latestCandle.volume);
    }

    // Detect swing points, BOS, CHoCH, and trendline breaks as before
    auto swingPoints = detectSwingPoints(candles);
    auto bosPoints = detectBOS(candles, swingPoints);
    auto chochPoints = detectCHoCH(candles, swingPoints);
    auto trendBreaks = detectTrendlineBreak(candles, swingPoints);

    // Remove structure update logging from here, as you requested

    if (lastOrderBlockDate == latestCandle.date)
    {
        spdlog::info("No new order block detected for today.");
        return;
    }

    auto structureEvents = StructureUtils::gatherStructureEvents(bosPoints, chochPoints, trendBreaks, candles);

    auto orderBlocks = detectOrderBlocks(candles);
    if (orderBlocks.empty())
    {
        spdlog::info(" No order blocks detected.");
        return;
    }

    auto latestOrderBlock = orderBlocks.back();
    std::string latestDate = latestOrderBlock.first.date;

    if (latestDate > lastOrderBlockDate)
    {
        lastOrderBlockDate = latestDate;
        const OBZone &ob = latestOrderBlock.first;
        const std::string &obType = latestOrderBlock.second;
        bool isBuy = (obType == "Bullish");

        OrderBlock obBlock = TradingUtils::createOrderBlockFromZone(ob, obType);
        double currentClose = candles.back().close;
        bool isBullishOB = (obType == "Bullish");

        obBlock.updateStrength(structureEvents, currentClose, isBullishOB);

        // Find entry price inside the OB zone after detection date
        double entryPrice = obBlock.entryPrice;
        bool foundEntry = false;
        for (const auto &candle : candles)
        {
            if (candle.date > latestDate &&
                candle.close >= std::min(ob.bottom, ob.top) &&
                candle.close <= std::max(ob.bottom, ob.top))
            {
                entryPrice = candle.close;
                foundEntry = true;
                break;
            }
        }
        obBlock.entryPrice = entryPrice;

        if (!foundEntry)
        {
            spdlog::warn(" No candle close found inside OB zone post-date; using boundary.");
        }

        LoggingUtils::logOrderBlockInfo(obBlock, ob, obType, latestDate, foundEntry, entryPrice, candles);

        // Pass ATR period, e.g., 14
        double atr = TradingUtils::calculateATR(candles, 14);
        if (atr > 0)
        {
            TradingUtils::logRiskManagement(entryPrice, ob, isBuy, atr);
        }
        else
        {
            spdlog::warn(" ATR calculation failed or returned 0.");
        }
    }
}
