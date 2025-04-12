#ifndef ORDERBLOCK_H
#define ORDERBLOCK_H

#include <vector>
#include <string>
#include <algorithm>
#include "Candle.h"
#include "MarketStructure.h" // Needed for StructurePoint & StructureType

// ========================
// ENUMS
// ========================

enum class OBType {
    Bullish,
    Bearish
};

enum class ConfirmationType {
    None,
    CHoCH,
    BOS
};

// ========================
// STRUCTURES
// ========================

struct OBZone {
    double top;            // High or open (whichever is higher)
    double bottom;         // Low or open (whichever is lower)
    std::string date;      // The date the order block was detected
    OBType type;           // Type of the Order Block (Bullish or Bearish)
    double score = 0.0;    // Optional: confidence/strength score
};

struct ConfirmedOB {
    OBZone zone;
    std::string direction;           // "Bullish" or "Bearish"
    ConfirmationType confirmation;  // CHoCH or BOS
    std::string confirmationDate;   // Date when confirmation occurred
};

// ========================
// Order Block Logic
// ========================

// Check if the current and previous candles form a strong bullish impulse
bool isStrongBullishImpulse(const Candle& prev, const Candle& curr);

// Get bullish order block zone from a candle
OBZone getBullishOBZone(const Candle& ob);

// Detect bullish order blocks from candles
std::vector<OBZone> findBullishOrderBlocks(const std::vector<Candle>& candles);

// Check if the current and previous candles form a strong bearish impulse
bool isStrongBearishImpulse(const Candle& prev, const Candle& curr);

// Get bearish order block zone from a candle
OBZone getBearishOBZone(const Candle& ob);

// Detect bearish order blocks from candles
std::vector<OBZone> findBearishOrderBlocks(const std::vector<Candle>& candles);

// Detect both bullish and bearish order blocks in the given candle vector
std::vector<std::pair<OBZone, std::string>> detectOrderBlocks(const std::vector<Candle>& candles);

// Filter and confirm OBs based on CHoCH and BOS structure points
std::vector<ConfirmedOB> filterOrderBlocksWithStructure(
    const std::vector<Candle>& candles,
    const std::vector<std::pair<OBZone, std::string>>& rawOBs,
    const std::vector<StructurePoint>& choch,
    const std::vector<StructurePoint>& bos
);

#endif // ORDERBLOCK_H
