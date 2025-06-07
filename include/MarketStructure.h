#ifndef MARKETSTRUCTURE_H
#define MARKETSTRUCTURE_H

#include <vector>
#include <string>
#include "Candle.h"

enum class StructureType {
    SwingHigh,
    SwingLow,
    BOS,
    CHoCH,
    TrendlineBreak,
    None
};

struct StructurePoint {
    std::string date;
    double price;
    StructureType type;
    size_t index;
};

std::vector<StructurePoint> detectSwingPoints(const std::vector<Candle>& candles, int lookback = 2);
std::vector<StructurePoint> detectBOS(const std::vector<Candle>& candles, const std::vector<StructurePoint>& swingPoints);
std::vector<StructurePoint> detectCHoCH(const std::vector<Candle>& candles, const std::vector<StructurePoint>& swingPoints, double retraceThreshold = 0.02);
std::vector<StructurePoint> detectTrendlineBreak(const std::vector<Candle>& candles, const std::vector<StructurePoint>& swingPoints, double threshold = 0.02);
std::vector<StructurePoint> detectStructure(const std::vector<Candle>& candles, StructureType type, double retraceThreshold = 0.02);

#endif // MARKETSTRUCTURE_H
