#ifndef DATAREADER_H
#define DATAREADER_H

#include <string>
#include <vector>
#include "Candle.h"

class DataReader {
public:
    // Constructor now accepts apiKey optionally
    DataReader(const std::string& filepath, const std::string& dataSource, const std::string& apiEndpoint = "", const std::string& apiKey = "");

    std::vector<Candle> readData();

private:
    std::string filepath;
    std::string dataSource;   // Either "CSV" or "API"
    std::string apiEndpoint;  // For API fetching
    std::string apiKey;       // API key stored securely

    std::vector<Candle> readCSV();
    std::vector<Candle> readAPI();
};

#endif // DATAREADER_H
