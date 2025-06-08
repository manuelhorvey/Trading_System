#include "TradingUtils.h"
#include <spdlog/spdlog.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

namespace TradingUtils
{
    std::string getTrendDirection(const std::vector<StructurePoint> &structurePoints)
    {
        std::vector<StructurePoint> swingHighs, swingLows;

        // Separate swing highs and swing lows
        for (const auto &sp : structurePoints)
        {
            if (sp.type == StructureType::SwingHigh)
                swingHighs.push_back(sp);
            else if (sp.type == StructureType::SwingLow)
                swingLows.push_back(sp);
        }

        // Need at least two swing highs and two swing lows to determine trend
        if (swingHighs.size() < 2 || swingLows.size() < 2)
            return "undefined";

        // Get the last two highs and lows
        const auto &lastHigh = swingHighs[swingHighs.size() - 1];
        const auto &prevHigh = swingHighs[swingHighs.size() - 2];
        const auto &lastLow = swingLows[swingLows.size() - 1];
        const auto &prevLow = swingLows[swingLows.size() - 2];

        // Compare highs and lows to classify trend direction
        if (lastHigh.price > prevHigh.price && lastLow.price > prevLow.price)
            return "uptrend";
        else if (lastHigh.price < prevHigh.price && lastLow.price < prevLow.price)
            return "downtrend";
        else
            return "sideways";
    }

    // New overload: trend from Candles
    std::string getTrendDirection(const std::vector<Candle> &candles)
    {
        if (candles.size() < 2)
            return "undefined";

        double lastClose = candles.back().close;
        double prevClose = candles[candles.size() - 2].close;

        if (lastClose > prevClose)
            return "uptrend";
        else if (lastClose < prevClose)
            return "downtrend";
        else
            return "sideways";
    }

    double calculateATR(const std::vector<Candle> &candles, size_t period)
    {
        if (candles.size() < period + 1)
            return 0.0;

        double atrSum = 0.0;

        for (size_t i = candles.size() - period; i < candles.size(); ++i)
        {
            double high = candles[i].high;
            double low = candles[i].low;
            double prevClose = candles[i - 1].close;

            double tr1 = high - low;
            double tr2 = std::fabs(high - prevClose);
            double tr3 = std::fabs(low - prevClose);

            double trueRange = std::max({tr1, tr2, tr3});
            atrSum += trueRange;
        }

        return atrSum / static_cast<double>(period);
    }

    void logRiskManagement(double entryPrice, const OBZone &orderBlock, bool isBuy, double atr,
                           double riskATRMultiplier, double rewardRiskRatioTP2, double rewardRiskRatioTP1)
    {
        double risk = atr * riskATRMultiplier;
        double sl = isBuy ? entryPrice - risk : entryPrice + risk;
        double tp1 = isBuy ? entryPrice + risk * rewardRiskRatioTP1 : entryPrice - risk * rewardRiskRatioTP1;
        double tp2 = isBuy ? entryPrice + risk * rewardRiskRatioTP2 : entryPrice - risk * rewardRiskRatioTP2;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "[RiskManagement] Entry: " << entryPrice
                  << (isBuy ? " BUY " : " SELL ")
                  << ", ATR: " << atr
                  << ", SL: " << sl
                  << ", TP1: " << tp1
                  << ", TP2: " << tp2
                  << std::endl;
    }

    OrderBlock createOrderBlockFromZone(const OBZone &obZone, const std::string &typeStr)
    {
        OBType obType;
        if (typeStr == "Bullish" || typeStr == "bullish")
            obType = OBType::Bullish;
        else if (typeStr == "Bearish" || typeStr == "bearish")
            obType = OBType::Bearish;
        else
            throw std::invalid_argument("Invalid OBType string: " + typeStr);

        double entryPrice = (obZone.top + obZone.bottom) / 2.0;

        OrderBlock ob(obZone.date, obZone.top, obZone.bottom, entryPrice, obType);

        return ob;
    }

    void logCandleWithDelta(const Candle &curr, const Candle *prev)
    {
        if (prev)
        {
            double delta = curr.volume - prev->volume;
            spdlog::info("Candle[Date:{}, O:{}, H:{}, L:{}, C:{}, Vol:{}, ΔVol:{}]",
                         curr.date, curr.open, curr.high, curr.low, curr.close,
                         curr.volume, delta);
        }
        else
        {
            spdlog::info("Candle[Date:{}, O:{}, H:{}, L:{}, C:{}, Vol:{}]",
                         curr.date, curr.open, curr.high, curr.low, curr.close, curr.volume);
        }
    }

    void logLastNCandles(const std::vector<Candle> &candles, size_t n)
    {
        if (candles.empty())
            return;

        size_t start = candles.size() > n ? candles.size() - n : 0;

        for (size_t i = start; i < candles.size(); ++i)
        {
            const Candle *prev = (i > 0) ? &candles[i - 1] : nullptr;
            logCandleWithDelta(candles[i], prev);
        }
    }

}
