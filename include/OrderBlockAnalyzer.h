#pragma once
#include <string>
#include "Config.h"
#include "DataReader.h"

class OrderBlockAnalyzer
{
    DataReader reader;
    std::string lastOrderBlockDate;

public:
    explicit OrderBlockAnalyzer(const Config &config);
    void analyze();
};
