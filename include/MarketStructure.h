#ifndef MARKETSTRUCTURE_H
#define MARKETSTRUCTURE_H

#include <vector>
#include <string>
#include "Candle.h"

// Enum for structure type
enum class StructureType {
    SwingHigh,
    SwingLow,
    BreakOfStructure,
    ChangeOfCharacter,
    CHoCH,
    BOS,
};

// Structure Point
struct StructurePoint {
    std::string date;
    double price;
    StructureType type;
    size_t index; // index in the candles vector
};

// Detect swing highs and lows
std::vector<StructurePoint> detectSwingPoints(const std::vector<Candle>& candles, int lookback = 2);

// Detect break of structure
std::vector<StructurePoint> detectBOS(
    const std::vector<Candle>& candles,
    const std::vector<StructurePoint>& swingPoints
);

// Detect Change of Charater
std::vector<StructurePoint> detectCHoCH(
    const std::vector<Candle>& candles,
    const std::vector<StructurePoint>& swingPoints
);


#endif // MARKETSTRUCTURE_H
