// MarketStructure.cpp
#include "MarketStructure.h"
#include "Candle.h"

// Detect swing highs and lows
std::vector<StructurePoint> detectSwingPoints(const std::vector<Candle>& candles, int lookback) {
    std::vector<StructurePoint> swingPoints;

    for (size_t i = lookback; i < candles.size() - lookback; ++i) {
        bool isSwingHigh = true;
        bool isSwingLow = true;

        // Check for swing high
        for (int j = 1; j <= lookback; ++j) {
            if (candles[i].high <= candles[i - j].high || candles[i].high <= candles[i + j].high) {
                isSwingHigh = false;
                break;
            }
        }

        // Check for swing low
        for (int j = 1; j <= lookback; ++j) {
            if (candles[i].low >= candles[i - j].low || candles[i].low >= candles[i + j].low) {
                isSwingLow = false;
                break;
            }
        }

        // Add to swing points
        if (isSwingHigh) {
            swingPoints.push_back({candles[i].date, candles[i].high, StructureType::SwingHigh, i});
        } else if (isSwingLow) {
            swingPoints.push_back({candles[i].date, candles[i].low, StructureType::SwingLow, i});
        }
    }

    return swingPoints;
}

// Detect break of structure (BOS)
std::vector<StructurePoint> detectBOS(
    const std::vector<Candle>& candles,
    const std::vector<StructurePoint>& swingPoints
) {
    std::vector<StructurePoint> bosPoints;

    for (size_t i = 1; i < candles.size(); ++i) {
        for (const auto& swing : swingPoints) {
            if (swing.type == StructureType::SwingHigh && candles[i].close > swing.price) {
                bosPoints.push_back({candles[i].date, candles[i].close, StructureType::BOS, i});
            } else if (swing.type == StructureType::SwingLow && candles[i].close < swing.price) {
                bosPoints.push_back({candles[i].date, candles[i].close, StructureType::BOS, i});
            }
        }
    }

    return bosPoints;
}


// Detect Change of Character (CHoCH)
std::vector<StructurePoint> detectCHoCH(
    const std::vector<Candle>& candles,
    const std::vector<StructurePoint>& swingPoints
) {
    std::vector<StructurePoint> chochPoints;

    for (size_t i = 1; i < candles.size(); ++i) {
        bool isCHoCH = false;
        StructurePoint point;

        // Check for a change from a bullish to bearish structure (or vice versa)
        for (const auto& swing : swingPoints) {
            if (swing.type == StructureType::SwingHigh && candles[i].close < swing.price) {
                point = {candles[i].date, candles[i].close, StructureType::CHoCH, i};
                isCHoCH = true;
            } else if (swing.type == StructureType::SwingLow && candles[i].close > swing.price) {
                point = {candles[i].date, candles[i].close, StructureType::CHoCH, i};
                isCHoCH = true;
            }
        }

        if (isCHoCH) {
            chochPoints.push_back(point);
        }
    }

    return chochPoints;
}
