#ifndef TRADE_H
#define TRADE_H

#include <string>

struct Trade {
    bool isLong;
    std::string symbol;
    std::string entryDate;
    double entryPrice;
    std::string exitDate;
    double exitPrice;
    double stopLoss;
    double takeProfit;
    double profit;
    double riskRewardRatio;

    // Constructor
    Trade(bool isLong,
          const std::string& symbol,
          const std::string& entryDate,
          double entryPrice,
          const std::string& exitDate,
          double exitPrice,
          double stopLoss = 0.0,
          double takeProfit = 0.0)
        : isLong(isLong),
          symbol(symbol),
          entryDate(entryDate),
          entryPrice(entryPrice),
          exitDate(exitDate),
          exitPrice(exitPrice),
          stopLoss(stopLoss),
          takeProfit(takeProfit)
    {
        profit = isLong ? (exitPrice - entryPrice) : (entryPrice - exitPrice);
        double risk = isLong ? (entryPrice - stopLoss) : (stopLoss - entryPrice);
        double reward = profit;
        riskRewardRatio = (risk > 0.0) ? (reward / risk) : 0.0;
    }
};

#endif // TRADE_H
