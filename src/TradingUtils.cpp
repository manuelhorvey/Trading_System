#include "TradingUtils.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdexcept>

namespace TradingUtils
{
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

    void logCandle(const Candle &c)
    {
        std::cout << "Candle[O:" << c.open << ", H:" << c.high << ", L:" << c.low << ", C:" << c.close << ", Vol:" << c.volume << "]" << std::endl;
    }
}
