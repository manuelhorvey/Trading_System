#pragma once
#include <vector>
#include <string>
#include "Candle.h"
#include "OrderBlock.h"

namespace TradingUtils
{
    std::string getTrendDirection(const std::vector<Candle> &candles);
    double calculateATR(const std::vector<Candle> &candles, size_t period = 14);
    void logRiskManagement(double entryPrice, const OBZone &orderBlock, bool isBuy, double atr,
                           double riskATRMultiplier = 1.5, double rewardRiskRatioTP2 = 2.0, double rewardRiskRatioTP1 = 1.0);
    OrderBlock createOrderBlockFromZone(const OBZone &obZone, const std::string &typeStr);
    void logCandle(const Candle &c);
}
