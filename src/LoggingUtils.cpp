#include "LoggingUtils.h"
#include <spdlog/spdlog.h>
#include "TradingUtils.h"  // For getTrendDirection and logCandlesInRange

void LoggingUtils::logOrderBlockInfo(const OrderBlock &obBlock, const OBZone &obZone, const std::string &obType,
                                     const std::string &latestDate, bool foundEntry, double entryPrice,
                                     const std::vector<Candle> &candles)
{
    spdlog::info("\n===  {} Order Block ===", obType);
    spdlog::info("| Date Detected  | {} |", latestDate);
    spdlog::info("| OB Zone        | Top: {:.2f} | Bottom: {:.2f} |", obZone.top, obZone.bottom);
    spdlog::info("| Entry Price    | {} | Close: {:.2f} |", foundEntry ? "Candle Close" : "Boundary", entryPrice);
    spdlog::info("| Strength       | {} |", obBlock.getStrengthString());

    // Get trend direction from the latest candles
    std::string trendDirection = TradingUtils::getTrendDirection(candles);
    spdlog::info("| Trend Direction      | {} |", trendDirection);

    
}


