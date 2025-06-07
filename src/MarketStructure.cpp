#include "MarketStructure.h"
#include <unordered_set>
#include <cmath>

// Helper to check significant retracement for CHoCH
static bool isSignificantRetrace(double price, double swingPrice, double retraceThreshold) {
    return (price > swingPrice * (1 + retraceThreshold)) || (price < swingPrice * (1 - retraceThreshold));
}

// Detect swing highs and lows
std::vector<StructurePoint> detectSwingPoints(const std::vector<Candle>& candles, int lookback) {
    std::vector<StructurePoint> swingPoints;

    for (size_t i = lookback; i < candles.size() - lookback; ++i) {
        bool isSwingHigh = true;
        bool isSwingLow = true;

        for (int j = 1; j <= lookback; ++j) {
            if (candles[i].high <= candles[i - j].high || candles[i].high <= candles[i + j].high) {
                isSwingHigh = false;
            }
            if (candles[i].low >= candles[i - j].low || candles[i].low >= candles[i + j].low) {
                isSwingLow = false;
            }
        }

        if (isSwingHigh) {
            swingPoints.push_back({candles[i].date, candles[i].high, StructureType::SwingHigh, i});
        }
        else if (isSwingLow) {
            swingPoints.push_back({candles[i].date, candles[i].low, StructureType::SwingLow, i});
        }
    }

    return swingPoints;
}

// Detect Break of Structure (BOS)
std::vector<StructurePoint> detectBOS(const std::vector<Candle>& candles, const std::vector<StructurePoint>& swingPoints) {
    std::vector<StructurePoint> bosPoints;
    std::unordered_set<size_t> triggered; // To avoid duplicate BOS from same swing point

    for (size_t i = 1; i < candles.size(); ++i) {
        for (const auto& swing : swingPoints) {
            if (triggered.count(swing.index)) continue;

            if (swing.type == StructureType::SwingHigh && candles[i].close > swing.price) {
                bosPoints.push_back({candles[i].date, candles[i].close, StructureType::BOS, i});
                triggered.insert(swing.index);
            }
            else if (swing.type == StructureType::SwingLow && candles[i].close < swing.price) {
                bosPoints.push_back({candles[i].date, candles[i].close, StructureType::BOS, i});
                triggered.insert(swing.index);
            }
        }
    }

    return bosPoints;
}

// Detect Change of Character (CHoCH)
std::vector<StructurePoint> detectCHoCH(const std::vector<Candle>& candles, const std::vector<StructurePoint>& swingPoints, double retraceThreshold) {
    std::vector<StructurePoint> chochPoints;

    for (size_t i = 1; i < candles.size(); ++i) {
        for (const auto& swing : swingPoints) {
            if (swing.type == StructureType::SwingHigh && candles[i].close < swing.price &&
                isSignificantRetrace(candles[i].close, swing.price, retraceThreshold)) {
                chochPoints.push_back({candles[i].date, candles[i].close, StructureType::CHoCH, i});
                break; // Avoid multiple CHoCH from same candle
            }
            else if (swing.type == StructureType::SwingLow && candles[i].close > swing.price &&
                isSignificantRetrace(candles[i].close, swing.price, retraceThreshold)) {
                chochPoints.push_back({candles[i].date, candles[i].close, StructureType::CHoCH, i});
                break;
            }
        }
    }

    return chochPoints;
}

// Detect trendline breaks between swing points of the same type
std::vector<StructurePoint> detectTrendlineBreak(
    const std::vector<Candle>& candles,
    const std::vector<StructurePoint>& swingPoints,
    double threshold
) {
    std::vector<StructurePoint> breaks;

    for (size_t i = 0; i < swingPoints.size(); ++i) {
        for (size_t j = i + 1; j < swingPoints.size(); ++j) {
            if ((swingPoints[i].type == StructureType::SwingHigh && swingPoints[j].type == StructureType::SwingHigh) ||
                (swingPoints[i].type == StructureType::SwingLow && swingPoints[j].type == StructureType::SwingLow)) {

                double x1 = static_cast<double>(swingPoints[i].index);
                double y1 = swingPoints[i].price;
                double x2 = static_cast<double>(swingPoints[j].index);
                double y2 = swingPoints[j].price;

                double slope = (y2 - y1) / (x2 - x1);

                for (size_t k = static_cast<size_t>(x1) + 1; k < static_cast<size_t>(x2); ++k) {
                    double trendPrice = y1 + slope * (k - x1);
                    double close = candles[k].close;

                    if (swingPoints[i].type == StructureType::SwingHigh && close > trendPrice * (1 + threshold)) {
                        breaks.push_back({candles[k].date, close, StructureType::TrendlineBreak, k});
                    }
                    else if (swingPoints[i].type == StructureType::SwingLow && close < trendPrice * (1 - threshold)) {
                        breaks.push_back({candles[k].date, close, StructureType::TrendlineBreak, k});
                    }
                }
            }
        }
    }

    return breaks;
}

// Generic detector routing
std::vector<StructurePoint> detectStructure(const std::vector<Candle>& candles, StructureType type, double retraceThreshold) {
    std::vector<StructurePoint> swingPoints = detectSwingPoints(candles, 2);

    switch (type) {
        case StructureType::CHoCH:
            return detectCHoCH(candles, swingPoints, retraceThreshold);
        case StructureType::BOS:
            return detectBOS(candles, swingPoints);
        case StructureType::TrendlineBreak:
            return detectTrendlineBreak(candles, swingPoints, retraceThreshold);
        case StructureType::SwingHigh:
        case StructureType::SwingLow:
            return swingPoints;
        default:
            return {};
    }
}
