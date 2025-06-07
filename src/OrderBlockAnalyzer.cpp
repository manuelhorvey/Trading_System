#include "OrderBlockAnalyzer.h"
#include "TradingUtils.h"
#include "StructureUtils.h"
#include "LoggingUtils.h"
#include <spdlog/spdlog.h>
#include <algorithm>

OrderBlockAnalyzer::OrderBlockAnalyzer(const Config &config)
    : reader(config.csv_path, config.data_source, config.api_endpoint, config.api_key)
{
    lastOrderBlockDate = "";
}

void OrderBlockAnalyzer::analyze()
{
    auto candles = reader.readData();

    if (candles.size() < 50)
    {
        spdlog::error("Not enough data (less than 50 bars available).");
        return;
    }

    const auto &mostRecentCandle = candles.back();
    std::string currentDate = mostRecentCandle.date;

    auto swingPoints = detectSwingPoints(candles);
    auto bosPoints = detectBOS(candles, swingPoints);
    auto chochPoints = detectCHoCH(candles, swingPoints);
    auto trendBreaks = detectTrendlineBreak(candles, swingPoints);

    spdlog::info(" Structure Update:");
    if (!bosPoints.empty() && bosPoints.back().date > lastOrderBlockDate)
    {
        spdlog::info(" ├─ BOS     → {} @ {:.2f}", bosPoints.back().date, bosPoints.back().price);
    }
    if (!chochPoints.empty() && chochPoints.back().date > lastOrderBlockDate)
    {
        spdlog::info(" ├─ CHoCH   → {} @ {:.2f}", chochPoints.back().date, chochPoints.back().price);
    }
    if (!trendBreaks.empty() && trendBreaks.back().date > lastOrderBlockDate)
    {
        spdlog::info(" └─ Trendline Break → {} @ {:.2f}", trendBreaks.back().date, trendBreaks.back().price);
    }

    if (lastOrderBlockDate == currentDate)
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
        
        double atr = TradingUtils::calculateATR(candles);
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
