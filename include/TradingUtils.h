#pragma once
#include <vector>
#include <string>
#include "Candle.h"
#include "OrderBlock.h"
#include "MarketStructure.h" 

namespace TradingUtils
{
    // Existing function based on candle closes
    std::string getTrendDirection(const std::vector<Candle> &candles);

    // New function based on swing points (StructurePoint)
    std::string getTrendDirection(const std::vector<StructurePoint> &structurePoints);

    double calculateATR(const std::vector<Candle> &candles, size_t period = 14);

    void logRiskManagement(double entryPrice, const OBZone &orderBlock, bool isBuy, double atr,
                           double riskATRMultiplier = 1.5, double rewardRiskRatioTP2 = 2.0, double rewardRiskRatioTP1 = 1.0);

    OrderBlock createOrderBlockFromZone(const OBZone &obZone, const std::string &typeStr);

    void logCandleWithDelta(const Candle &curr, const Candle *prev = nullptr);
}
