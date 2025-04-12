#include "Strategy.h"
#include <iostream>
#include <algorithm>
#include "Order.h"

// Constructor: Accepts candles and structure points (CHoCH, BOS)
Strategy::Strategy(const std::vector<Candle>& candles, const std::vector<StructurePoint>& choch, const std::vector<StructurePoint>& bos)
    : candles(candles), choch(choch), bos(bos) {}

// Main strategy runner
void Strategy::run() {
    detectAndGenerateOrders();
    confirmAndFilterOrderBlocks();
}

// Method to get generated orders
const std::vector<Order>& Strategy::getOrders() const {
    return orders;
}

// Detect order blocks and generate orders
void Strategy::detectAndGenerateOrders() {
    // Detect both bullish and bearish order blocks
    auto rawOrderBlocks = detectOrderBlocks(candles);
    
    for (const auto& ob : rawOrderBlocks) {
        const OBZone& zone = ob.first;
        const std::string& direction = ob.second;

        // Log detected order block
        std::cout << "Detected Order Block: " << direction << " at " << zone.date 
                  << " | Zone Top: " << zone.top << " | Zone Bottom: " << zone.bottom << std::endl;

        // Generate the order based on the order block's direction
        if (direction == "Bullish") {
            generateBuyOrder(zone);
        } else if (direction == "Bearish") {
            generateSellOrder(zone);
        }
    }
}

// Generate a buy order for bullish order blocks
void Strategy::generateBuyOrder(const OBZone& ob) {
    // Example: Buy at the close of the confirmed OB
    double takeProfit = ob.top + (ob.top - ob.bottom) * 2;  // Example TP (you can adjust this formula)
    double stopLoss = ob.bottom;  // Example SL (you can adjust this)

    // Create buy order and push it into the orders vector
    Order order(Order::Type::BUY, ob.top, ob.date, stopLoss, takeProfit);
    orders.push_back(order);

    // Log the order generation
    std::cout << "Generated Buy Order at " << ob.top << " on " << ob.date << " | Stop Loss: " 
              << stopLoss << " | Take Profit: " << takeProfit << std::endl;
}

// Generate a sell order for bearish order blocks
void Strategy::generateSellOrder(const OBZone& ob) {
    // Example: Sell at the close of the confirmed OB
    double takeProfit = ob.bottom - (ob.top - ob.bottom) * 2;  // Example TP (you can adjust this formula)
    double stopLoss = ob.top;  // Example SL (you can adjust this)

    // Create sell order and push it into the orders vector
    Order order(Order::Type::SELL, ob.bottom, ob.date, stopLoss, takeProfit);
    orders.push_back(order);

    // Log the order generation
    std::cout << "Generated Sell Order at " << ob.bottom << " on " << ob.date << " | Stop Loss: " 
              << stopLoss << " | Take Profit: " << takeProfit << std::endl;
}

// Filter and confirm order blocks with structure points (CHoCH, BOS)
void Strategy::confirmAndFilterOrderBlocks() {
    // Use structure points (CHoCH, BOS) to validate and filter order blocks
    std::vector<ConfirmedOB> confirmedOrderBlocks = filterOrderBlocksWithStructure(
        candles, detectOrderBlocks(candles), choch, bos
    );

    // Log confirmed order blocks
    for (const auto& confirmedOB : confirmedOrderBlocks) {
        std::cout << "Confirmed " << (confirmedOB.zone.type == OBType::Bullish ? "Bullish" : "Bearish")
                  << " OB at " << confirmedOB.zone.top << " with confirmation on " 
                  << confirmedOB.confirmationDate << std::endl;
    }
}
