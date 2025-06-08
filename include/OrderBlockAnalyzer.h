#pragma once
#include <string>
#include <vector>
#include "Config.h"
#include "DataReader.h"
#include "MarketStructure.h"

class OrderBlockAnalyzer
{
    DataReader reader;
    std::string lastOrderBlockDate;
    std::string lastBOSDate;
    std::string lastCHoCHDate;
    std::string lastTrendBreakDate;

    std::vector<StructurePoint> recentSwingPoints;

public:
    explicit OrderBlockAnalyzer(const Config &config);

    // Runs full analysis, including detecting swings and order blocks
    void analyze();

    // Returns the latest detected swing points for external use (read-only)
    const std::vector<StructurePoint>& getSwingPoints() const { return recentSwingPoints; }
};
