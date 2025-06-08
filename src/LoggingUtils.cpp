#include "LoggingUtils.h"
#include <spdlog/spdlog.h>
#include "TradingUtils.h"

void LoggingUtils::logOrderBlockInfo(const OrderBlock &obBlock, const OBZone &obZone, const std::string &obType,
                                     const std::string &latestDate, bool foundEntry, double entryPrice,
                                     const std::vector<Candle> &candles)
{
    spdlog::info("\n===  {} Order Block ===", obType);
    spdlog::info("| Date Detected        | {} |", latestDate);
    spdlog::info("| OB Zone              | Top: {:.2f} | Bottom: {:.2f} |", obZone.top, obZone.bottom);
    spdlog::info("| Entry Price          | {} | Close: {:.2f} |", foundEntry ? "Candle Close" : "Boundary", entryPrice);
    spdlog::info("| Strength             | {} |", obBlock.getStrengthString());
    spdlog::info("| Score                | {:.2f} |", obBlock.score);

    std::string trendDirection = TradingUtils::getTrendDirection(candles);
    spdlog::info("| Trend Direction      | {} |", trendDirection);

    spdlog::info("| Recent Candles (last 5) |");

    const int n = static_cast<int>(candles.size());
    const int start = std::max(0, n - 5);
    for (int i = start; i < n; ++i)
    {
        const Candle &curr = candles[i];
        const Candle *prev = (i > 0) ? &candles[i - 1] : nullptr;
        TradingUtils::logCandleWithDelta(curr, prev);
    }
}