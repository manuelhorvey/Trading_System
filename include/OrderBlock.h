#ifndef ORDERBLOCK_H
#define ORDERBLOCK_H

#include <vector>
#include <string>
#include <algorithm>
#include "Candle.h"
#include "MarketStructure.h"

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
    double top;                    // OB upper boundary
    double bottom;                 // OB lower boundary
    std::string date;             // Detection date
    OBType type;                  // Bullish or Bearish
    double score = 0.0;           // Scoring (confluence, reaction, structure)
};

struct ConfirmedOB {
    OBZone zone;
    std::string direction;              // Market direction: "Bullish" / "Bearish"
    ConfirmationType confirmation;      // CHoCH / BOS confirmation
    std::string confirmationDate;
};

struct StructureEvent {
    StructureEventType type;
    std::string date;
    double price;

    StructureEvent(StructureEventType t = StructureEventType::None,
                   const std::string& d = "",
                   double p = 0.0)
        : type(t), date(d), price(p) {}
};

// ========================
// Order Block Class
// ========================

class OrderBlock {
public:
    std::string date;
    double top;
    double bottom;
    double entryPrice;
    OBType type;
    OBStrength strength;
    double score;

    OrderBlock(const std::string& d, double t, double b, double e, OBType obType, double s = 0.0)
        : date(d), top(t), bottom(b), entryPrice(e), type(obType), strength(OBStrength::Valid), score(s) {}

    // Construct from OBZone directly
    OrderBlock(const OBZone& zone, double entry)
        : date(zone.date), top(zone.top), bottom(zone.bottom),
          entryPrice(entry), type(zone.type), strength(OBStrength::Valid), score(zone.score) {}

    // Update OB strength using structure events
    void updateStrength(const std::vector<StructureEvent>& events, double currentClose, bool isBullishOB);

    // Helper to convert strength enum to string
    std::string getStrengthString() const;
};

// ========================
// Detection & Filtering Functions
// ========================

bool isStrongBullishImpulse(const Candle& prev, const Candle& curr);
bool isStrongBearishImpulse(const Candle& prev, const Candle& curr);

OBZone getBullishOBZone(const Candle& ob);
OBZone getBearishOBZone(const Candle& ob);

std::vector<OBZone> findBullishOrderBlocks(const std::vector<Candle>& candles);
std::vector<OBZone> findBearishOrderBlocks(const std::vector<Candle>& candles);

std::vector<std::pair<OBZone, std::string>> detectOrderBlocks(const std::vector<Candle>& candles);

std::vector<ConfirmedOB> filterOrderBlocksWithStructure(
    const std::vector<Candle>& candles,
    const std::vector<std::pair<OBZone, std::string>>& rawOBs,
    const std::vector<StructurePoint>& choch,
    const std::vector<StructurePoint>& bos
);

#endif // ORDERBLOCK_H
