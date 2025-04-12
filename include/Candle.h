#ifndef CANDLE_H
#define CANDLE_H

#include <string>
#include <cmath>

class Candle {
public:
    double open;
    double high;
    double low;
    double close;
    int volume;
    std::string date;
    double changePercent;

    // Default Constructor
    Candle() 
        : open(0.0), high(0.0), low(0.0), close(0.0), volume(0), date(""), changePercent(0.0) {}

    // Constructor with all parameters
    Candle(double o, double h, double l, double c, int v, const std::string& d, double cp)
        : open(o), high(h), low(l), close(c), volume(v), date(d), changePercent(cp) {}

    // Determine if the candle is bullish
    bool isBullish() const {
        return close > open;
    }

    // Determine if the candle is bearish
    bool isBearish() const {
        return open > close;
    }

    // Get the body size of the candle (absolute difference between open and close)
    double bodySize() const {
        return std::abs(close - open);
    }

    // Get the range of the candle (difference between high and low)
    double candleRange() const {
        return high - low;
    }

    // Get the upper wick size
    double upperWick() const {
        return isBullish() ? high - close : high - open;
    }

    // Get the lower wick size
    double lowerWick() const {
        return isBullish() ? open - low : close - low;
    }

    // Get a string representation of the candle for logging or debugging
    std::string toString() const {
        return "Date: " + date + ", Open: " + std::to_string(open) +
               ", High: " + std::to_string(high) + ", Low: " + std::to_string(low) +
               ", Close: " + std::to_string(close) + ", Volume: " + std::to_string(volume) +
               ", Change %: " + std::to_string(changePercent);
    }
};

#endif // CANDLE_H
