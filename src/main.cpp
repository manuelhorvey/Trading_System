#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "DataReader.h"
#include "OrderBlock.h"
#include "json.hpp"
#include "Candle.h"
#include "MarketStructure.h"
#include <spdlog/spdlog.h>

using json = nlohmann::json;

// struct Trade {
//     std::string entryDate;
//     std::string exitDate;
//     double entryPrice;
//     double exitPrice;
//     bool isLong;
//     double profit; // profit ratio (e.g., 0.02 for 2%)
// };

// void simulateBacktest(const std::vector<Candle>& candles, 
//     const std::vector<ConfirmedOB>& confirmedOBs) 
// {
//     std::unordered_map<std::string, size_t> candleIndex;
//     for (size_t i = 0; i < candles.size(); i++) {
//         candleIndex[candles[i].date] = i;
//     }

//     std::vector<Trade> trades;

//     for (const auto& ob : confirmedOBs) {
//         if (candleIndex.find(ob.zone.date) == candleIndex.end())
//             continue;
//         size_t obIdx = candleIndex[ob.zone.date];

//         bool entered = false;
//         Trade trade;
//         trade.isLong = (ob.direction == "Bullish");

//         for (size_t i = obIdx + 1; i < candles.size(); i++) {
//             const Candle& c = candles[i];
//             if (trade.isLong && c.low <= ob.zone.bottom) {
//                 trade.entryPrice = ob.zone.bottom;
//                 trade.entryDate = c.date;
//                 entered = true;
//                 break;
//             }
//             if (!trade.isLong && c.high >= ob.zone.top) {
//                 trade.entryPrice = ob.zone.top;
//                 trade.entryDate = c.date;
//                 entered = true;
//                 break;
//             }
//         }
//         if (!entered) continue;

//         double stopLossPercent = 0.03;
//         double riskRewardRatio = 14.0;
//         double takeProfitPercent = stopLossPercent * riskRewardRatio;

//         double stop = trade.entryPrice * (1.0 - stopLossPercent);
//         double target = trade.entryPrice * (1.0 + takeProfitPercent);
//         double highestPrice = trade.entryPrice;
//         double lowestPrice = trade.entryPrice;

//         size_t entryIdx = candleIndex[trade.entryDate];
//         bool exited = false;

//         for (size_t i = entryIdx + 1; i < candles.size() && i < entryIdx + 6; i++) {
//             const Candle& c = candles[i];
//             if (trade.isLong) {
//                 if (c.high > highestPrice) {
//                     highestPrice = c.high;
//                     stop = std::max(stop, highestPrice * (1.0 - stopLossPercent));  // Update stop to trail the price
//                 }
//                 if (c.low <= stop) {
//                     trade.exitPrice = stop;
//                     trade.exitDate = c.date;
//                     exited = true;
//                     break;
//                 }
//                 if (c.high >= target) {
//                     trade.exitPrice = target;
//                     trade.exitDate = c.date;
//                     exited = true;
//                     break;
//                 }
//             } else {
//                 if (c.low < lowestPrice) {
//                     lowestPrice = c.low;
//                     stop = std::min(stop, lowestPrice * (1.0 + stopLossPercent));  // Update stop to trail the price
//                 }
//                 if (c.high >= stop) {
//                     trade.exitPrice = stop;
//                     trade.exitDate = c.date;
//                     exited = true;
//                     break;
//                 }
//                 if (c.low <= target) {
//                     trade.exitPrice = target;
//                     trade.exitDate = c.date;
//                     exited = true;
//                     break;
//                 }
//             }
//         }

//         if (!exited && entryIdx + 5 < candles.size()) {
//             const Candle& lastC = candles[entryIdx + 5];
//             trade.exitPrice = lastC.close;
//             trade.exitDate = lastC.date;
//         } else if (!exited) {
//             continue;
//         }

//         // Check for zero or invalid prices before calculating profit
//         if (trade.entryPrice == 0 || trade.exitPrice == 0) {
//             spdlog::error("Error: Zero price encountered for trade (Entry: {}, Exit: {}). Skipping trade.", trade.entryPrice, trade.exitPrice);
//             continue;
//         }

//         if (trade.isLong)
//             trade.profit = (trade.exitPrice - trade.entryPrice) / trade.entryPrice;
//         else
//             trade.profit = (trade.entryPrice - trade.exitPrice) / trade.entryPrice;

//         if (std::isnan(trade.profit)) {
//             spdlog::error("Error: NaN profit encountered for trade.");
//             continue;
//         }

//         trades.push_back(trade);
//     }

//     double cumulativeProfit = 0.0;
//     spdlog::info("\n--- Backtest Trade Results ---");
//     for (const auto& t : trades) {
//         spdlog::info("{} | Entry: {} @ {} | Exit: {} @ {} | Profit: {}%", 
//             (t.isLong ? "Long" : "Short"), 
//             t.entryDate, t.entryPrice, t.exitDate, t.exitPrice, t.profit * 100);
//         cumulativeProfit += t.profit;
//     }
//     spdlog::info("Total trades: {}", trades.size());
//     spdlog::info("Cumulative Profit: {}%", cumulativeProfit * 100);
// }

void loopThroughAllDataWithOrderBlocks(DataReader& reader) {
    std::vector<Candle> candles = reader.readData();

    if (candles.size() < 50) {
        spdlog::error("Not enough data (less than 50 bars available).");
        return;
    }

    std::vector<std::pair<OBZone, std::string>> orderBlocks = detectOrderBlocks(candles);
    std::unordered_map<std::string, std::pair<OBZone, std::string>> obMap;
    for (const auto& ob : orderBlocks) {
        obMap[ob.first.date] = ob;
    }

    std::vector<StructurePoint> swingPoints = detectSwingPoints(candles);
    std::vector<StructurePoint> choch = detectCHoCH(candles, swingPoints);
    std::vector<StructurePoint> bos = detectBOS(candles, swingPoints);

    // Get confirmed OBs with structure validation.
    std::vector<ConfirmedOB> confirmedOBs = filterOrderBlocksWithStructure(candles, orderBlocks, choch, bos);
    
    // (Optional) Print confirmed OBs.
    spdlog::info("\nConfirmed Order Blocks:");
    for (const auto& ob : confirmedOBs) {
        spdlog::info("{} {} / {} Confirmed by {} on {}", 
            ob.zone.date, ob.zone.top, ob.zone.bottom, 
            (ob.confirmation == ConfirmationType::CHoCH ? "CHoCH" : "BOS"),
            ob.confirmationDate);
    }
    
    // Print candle details and OB info.
    spdlog::info("\n--- Candle Data ---");
    for (size_t i = 0; i < candles.size(); ++i) {
        const Candle& bar = candles[i];
        spdlog::info("Date: {} | Open: {} | High: {} | Low: {} | Close: {} | Body Size: {} | % Change: {}%",
            bar.date, bar.open, bar.high, bar.low, bar.close, bar.bodySize(), bar.changePercent);
        if (obMap.count(bar.date)) {
            const OBZone& zone = obMap[bar.date].first;
            const std::string& type = obMap[bar.date].second;
            spdlog::info("  → {} Order Block detected at {} | Zone Top: {} | Zone Bottom: {}", 
                type, bar.date, zone.top, zone.bottom);
        }
        spdlog::info("--------------------------------------------------");
    }
    
    // // Run the backtest simulation.
    // simulateBacktest(candles, confirmedOBs);
}

int main() {
    std::ifstream configFile("../config/settings.json");
    if (!configFile.is_open()) {
        spdlog::error("Could not open config file.");
        return 1;
    }

    json config;
    configFile >> config;

    if (!config.contains("csv_path") || 
        !config.contains("data_source") || 
        !config.contains("api_endpoint") || 
        !config.contains("api_key")) {
        spdlog::error("Missing necessary fields in config file.");
        return 1;
    }

    std::string csvPath = config["csv_path"];
    std::string dataSource = config["data_source"];
    std::string apiEndpoint = config["api_endpoint"];
    std::string apiKey = config["api_key"];

    // Pass all required fields to the DataReader constructor
    DataReader reader(csvPath, dataSource, apiEndpoint, apiKey);

    spdlog::info("Running full dataset analysis...");
    try {
        loopThroughAllDataWithOrderBlocks(reader);
    } catch (const std::exception& e) {
        spdlog::error("Error occurred: {}", e.what());
    }

    spdlog::info("Program completed.");
    return 0;
}