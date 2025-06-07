#pragma once
#include "OrderBlock.h"
#include "Candle.h"
#include <vector>
#include <string>

namespace LoggingUtils
{
    void logOrderBlockInfo(const OrderBlock &obBlock, const OBZone &obZone, const std::string &obType,
                           const std::string &latestDate, bool foundEntry, double entryPrice,
                           const std::vector<Candle> &candles);
}
