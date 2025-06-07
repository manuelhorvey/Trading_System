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

enum class OBStrength {
    Valid,
    Weak,
    Invalidated
};

enum class StructureEventType {
    BOS_Bearish,
    BOS_Bullish,
    CHoCH_Bullish,
    CHoCH_Bearish,
    TrendlineBreak,
    None
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

struct StructureEvent {
    StructureEventType type;
    std::string date;
    double price;

    StructureEvent(StructureEventType t = StructureEventType::None, 
                   const std::string& d = "", 
                   double p = 0.0) : type(t), date(d), price(p) {}
};

// ========================
// Order Block Class for confluence & strength
// ========================

class OrderBlock {
public:
    std::string date;
    double top;
    double bottom;
    double entryPrice;
    OBType type;
    OBStrength strength;

    OrderBlock(const std::string& d, double t, double b, double e, OBType obType)
        : date(d), top(t), bottom(b), entryPrice(e), type(obType), strength(OBStrength::Valid) {}

    // Update OB strength based on structure events after OB date
    void updateStrength(const std::vector<StructureEvent>& events, double currentClose, bool isBullishOB);

    // Helper for logging strength as string
    std::string getStrengthString() const;
};

// ========================
// Order Block Logic Functions
// ========================

// Detect strong bullish impulse between two candles
bool isStrongBullishImpulse(const Candle& prev, const Candle& curr);

// Return bullish order block zone based on a candle
OBZone getBullishOBZone(const Candle& ob);

// Find bullish order blocks in a candle series
std::vector<OBZone> findBullishOrderBlocks(const std::vector<Candle>& candles);

// Detect strong bearish impulse between two candles
bool isStrongBearishImpulse(const Candle& prev, const Candle& curr);

// Return bearish order block zone based on a candle
OBZone getBearishOBZone(const Candle& ob);

// Find bearish order blocks in a candle series
std::vector<OBZone> findBearishOrderBlocks(const std::vector<Candle>& candles);

// Detect both bullish and bearish order blocks
std::vector<std::pair<OBZone, std::string>> detectOrderBlocks(const std::vector<Candle>& candles);

// Filter order blocks using CHoCH and BOS confirmations from structure points
std::vector<ConfirmedOB> filterOrderBlocksWithStructure(
    const std::vector<Candle>& candles,
    const std::vector<std::pair<OBZone, std::string>>& rawOBs,
    const std::vector<StructurePoint>& choch,
    const std::vector<StructurePoint>& bos
);

#endif // ORDERBLOCK_H
