#include "Order.h"
#include <iostream>
#include <vector>
#include <string>
#include "Candle.h"
#include "MarketStructure.h"
#include "OrderBlock.h"

class Strategy {
public:
    // Constructor to initialize candles, CHoCH, and BOS points
    Strategy(const std::vector<Candle>& candles, const std::vector<StructurePoint>& choch, const std::vector<StructurePoint>& bos);

    // Main strategy runner
    void run();

    // Method to get generated orders
    const std::vector<Order>& getOrders() const;

private:
    std::vector<Candle> candles;
    std::vector<StructurePoint> choch;
    std::vector<StructurePoint> bos;
    std::vector<Order> orders;

    // Detect order blocks and generate orders
    void detectAndGenerateOrders();

    // Generate a buy order for bullish order blocks
    void generateBuyOrder(const OBZone& ob);

    // Generate a sell order for bearish order blocks
    void generateSellOrder(const OBZone& ob);

    // Filter and confirm order blocks with structure points (CHoCH, BOS)
    void confirmAndFilterOrderBlocks();
};
