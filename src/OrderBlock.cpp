#include "OrderBlock.h"
#include <algorithm>

// Detect strong bullish impulse
bool isStrongBullishImpulse(const Candle& c1, const Candle& c2) {
    return c1.isBullish() && c2.isBullish() &&
           std::abs(c2.close - c2.open) > std::abs(c1.close - c1.open);
}

// Detect strong bearish impulse
bool isStrongBearishImpulse(const Candle& c1, const Candle& c2) {
    return c1.isBearish() && c2.isBearish() &&
           std::abs(c2.open - c2.close) > std::abs(c1.open - c1.close);
}

// Return the bullish order block zone
OBZone getBullishOBZone(const Candle& ob) {
    OBZone zone;
    zone.top = std::max(ob.high, ob.open);
    zone.bottom = std::min(ob.low, ob.close);
    zone.date = ob.date;
    zone.type = OBType::Bullish;

    double body = std::abs(ob.close - ob.open);
    double range = ob.high - ob.low;
    zone.score = (range > 0) ? body / range : 0.0;

    return zone;
}

// Return the bearish order block zone
OBZone getBearishOBZone(const Candle& ob) {
    OBZone zone;
    zone.top = std::max(ob.high, ob.open);
    zone.bottom = std::min(ob.low, ob.close);
    zone.date = ob.date;
    zone.type = OBType::Bearish;

    double body = std::abs(ob.close - ob.open);
    double range = ob.high - ob.low;
    zone.score = (range > 0) ? body / range : 0.0;

    return zone;
}

// Find bullish order blocks
std::vector<OBZone> findBullishOrderBlocks(const std::vector<Candle>& candles) {
    std::vector<OBZone> bullishOrderBlocks;

    for (size_t i = 2; i < candles.size(); ++i) {
        const Candle& ob = candles[i];
        const Candle& c1 = candles[i - 1];
        const Candle& c2 = candles[i - 2];

        if (ob.isBearish() && isStrongBullishImpulse(c1, c2)) {
            OBZone zone = getBullishOBZone(ob);
            bullishOrderBlocks.push_back(zone);
        }
    }

    return bullishOrderBlocks;
}

// Find bearish order blocks
std::vector<OBZone> findBearishOrderBlocks(const std::vector<Candle>& candles) {
    std::vector<OBZone> bearishOrderBlocks;

    for (size_t i = 2; i < candles.size(); ++i) {
        const Candle& ob = candles[i];
        const Candle& c1 = candles[i - 1];
        const Candle& c2 = candles[i - 2];

        if (ob.isBullish() && isStrongBearishImpulse(c1, c2)) {
            OBZone zone = getBearishOBZone(ob);
            bearishOrderBlocks.push_back(zone);
        }
    }

    return bearishOrderBlocks;
}

// Detect both bullish and bearish order blocks
std::vector<std::pair<OBZone, std::string>> detectOrderBlocks(const std::vector<Candle>& candles) {
    std::vector<std::pair<OBZone, std::string>> orderBlocks;

    for (size_t i = 0; i + 2 < candles.size(); ++i) {
        const Candle& ob = candles[i];
        const Candle& c1 = candles[i + 1];
        const Candle& c2 = candles[i + 2];

        if (ob.isBearish() && isStrongBullishImpulse(c1, c2)) {
            OBZone bullishZone = getBullishOBZone(ob);
            orderBlocks.emplace_back(bullishZone, "Bullish");
        }

        if (ob.isBullish() && isStrongBearishImpulse(c1, c2)) {
            OBZone bearishZone = getBearishOBZone(ob);
            orderBlocks.emplace_back(bearishZone, "Bearish");
        }
    }

    return orderBlocks;
}

// Confirm OBs with CHoCH and BOS structure
std::vector<ConfirmedOB> filterOrderBlocksWithStructure(
    const std::vector<Candle>& candles,
    const std::vector<std::pair<OBZone, std::string>>& rawOBs,
    const std::vector<StructurePoint>& choch,
    const std::vector<StructurePoint>& bos
) {
    std::vector<ConfirmedOB> confirmedOBs;

    for (const auto& [zone, dir] : rawOBs) {
        ConfirmationType confType = ConfirmationType::None;
        std::string confDate = "";

        for (const auto& ch : choch) {
            if (ch.type == StructureType::CHoCH &&
                ((zone.type == OBType::Bullish && ch.price > zone.top) ||
                 (zone.type == OBType::Bearish && ch.price < zone.bottom)) &&
                ch.index > 0)
            {
                confType = ConfirmationType::CHoCH;
                confDate = candles[ch.index].date;
                break;
            }
        }

        if (confType == ConfirmationType::None) {
            for (const auto& b : bos) {
                if (b.type == StructureType::BOS &&
                    ((zone.type == OBType::Bullish && b.price > zone.top) ||
                     (zone.type == OBType::Bearish && b.price < zone.bottom)) &&
                    b.index > 0)
                {
                    confType = ConfirmationType::BOS;
                    confDate = candles[b.index].date;
                    break;
                }
            }
        }

        if (confType != ConfirmationType::None) {
            ConfirmedOB ob;
            ob.zone = zone;
            ob.direction = dir;
            ob.confirmation = confType;
            ob.confirmationDate = confDate;
            confirmedOBs.push_back(ob);
        }
    }

    return confirmedOBs;
}

// ==============================
// OrderBlock class methods
// ==============================

std::string OrderBlock::getStrengthString() const {
    switch (strength) {
        case OBStrength::Valid: return "Valid";
        case OBStrength::Weak: return "Weak";
        case OBStrength::Invalidated: return "Invalidated";
    }
    return "Unknown";
}

// Update strength with structure events AND current candle close price check
void OrderBlock::updateStrength(const std::vector<StructureEvent>& events, double currentClose, bool isBullishOB) {
    strength = OBStrength::Valid;
    score = 1.0;  // Start with base score

    // Invalidation check
    if ((isBullishOB && currentClose < bottom) || (!isBullishOB && currentClose > top)) {
        strength = OBStrength::Invalidated;
        score = 0.0;
        return;
    }

    for (const auto& event : events) {
        if (event.date <= date) continue;

        // Scoring logic
        if (isBullishOB) {
            if (event.type == StructureEventType::BOS_Bullish || event.type == StructureEventType::CHoCH_Bullish) {
                if (event.price > top) score += 1.0;
            } else if (event.type == StructureEventType::BOS_Bearish || event.type == StructureEventType::CHoCH_Bearish) {
                if (event.price < top) score -= 0.5;
                if (event.price < bottom) {
                    strength = OBStrength::Invalidated;
                    score = 0.0;
                    return;
                }
            }
        } else {
            if (event.type == StructureEventType::BOS_Bearish || event.type == StructureEventType::CHoCH_Bearish) {
                if (event.price < bottom) score += 1.0;
            } else if (event.type == StructureEventType::BOS_Bullish || event.type == StructureEventType::CHoCH_Bullish) {
                if (event.price > bottom) score -= 0.5;
                if (event.price > top) {
                    strength = OBStrength::Invalidated;
                    score = 0.0;
                    return;
                }
            }
        }
    }

    if (score <= 0.5) {
        strength = OBStrength::Weak;
    } else if (score >= 2.0) {
        strength = OBStrength::Valid;
    }
}
