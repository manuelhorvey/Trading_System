#include "DataReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include "json.hpp"
#include "Candle.h"

using json = nlohmann::json;

// Updated constructor with apiKey
DataReader::DataReader(const std::string& filepath, const std::string& dataSource, const std::string& apiEndpoint, const std::string& apiKey)
    : filepath(filepath), dataSource(dataSource), apiEndpoint(apiEndpoint), apiKey(apiKey) {}

// Helper function to trim leading and trailing spaces
std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// Helper function to read CSV file
std::vector<Candle> DataReader::readCSV() {
    std::vector<Candle> candles;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return candles;
    }

    std::string line;
    std::vector<std::string> lines;

    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        lines.push_back(line);
    }

    file.close();
    std::reverse(lines.begin(), lines.end());

    for (const std::string& rawLine : lines) {
        std::stringstream ss(rawLine);
        std::string date, openStr, highStr, lowStr, closeStr, changeStr, percentChangeStr;
        
        if (rawLine.find("Date") != std::string::npos) continue; 

        std::getline(ss, date, ',');
        std::getline(ss, openStr, ',');
        std::getline(ss, highStr, ',');
        std::getline(ss, lowStr, ',');
        std::getline(ss, closeStr, ',');
        std::getline(ss, changeStr, ',');
        std::getline(ss, percentChangeStr);

        if (openStr.empty() || highStr.empty() || lowStr.empty() || closeStr.empty() || percentChangeStr.empty()) {
            std::cerr << "Skipping line due to missing fields: " << rawLine << std::endl;
            continue;
        }

        openStr = trim(openStr);
        highStr = trim(highStr);
        lowStr = trim(lowStr);
        closeStr = trim(closeStr);
        percentChangeStr = trim(percentChangeStr);

        Candle candle;
        candle.date = date;

        try {
            candle.open = std::stod(openStr);
            candle.high = std::stod(highStr);
            candle.low = std::stod(lowStr);
            candle.close = std::stod(closeStr);
            candle.changePercent = std::stod(percentChangeStr);
        } catch (const std::exception& e) {
            std::cerr << "Conversion error on line: " << rawLine << "\n"
                      << "  Reason: " << e.what() << std::endl;
            continue;
        }

        candles.push_back(candle);
    }

    std::cout << "Loaded " << candles.size() << " candles from CSV.\n";
    return candles;
}

// Helper function to handle API response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to read data from API
std::vector<Candle> DataReader::readAPI() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    std::vector<Candle> candles;

    curl = curl_easy_init();

    if (curl) {
        // Append API key dynamically
        std::string fullUrl = apiEndpoint + "&apikey=" + apiKey;

        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

        if (httpCode != 200) {
            std::cerr << "API call failed with HTTP code: " << httpCode << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return {};
    }

    // Parse JSON response
    try {
        json j = json::parse(readBuffer);
        std::string timeSeriesKey;

        // Find the key with time series data (e.g., "Time Series (5min)")
        for (auto& item : j.items()) {
            if (item.key().find("Time Series") != std::string::npos) {
                timeSeriesKey = item.key();
                break;
            }
        }

        if (timeSeriesKey.empty()) {
            std::cerr << "Could not find time series data in API response." << std::endl;
            return {};
        }

        const json& series = j[timeSeriesKey];

        for (auto it = series.begin(); it != series.end(); ++it) {
            const std::string& timestamp = it.key();
            const json& values = it.value();

            Candle candle;
            candle.date = timestamp;
            candle.open = std::stod(values["1. open"].get<std::string>());
            candle.high = std::stod(values["2. high"].get<std::string>());
            candle.low = std::stod(values["3. low"].get<std::string>());
            candle.close = std::stod(values["4. close"].get<std::string>());

            // Calculate percent change
            candle.changePercent = ((candle.close - candle.open) / candle.open) * 100.0;

            candles.push_back(candle);
        }

        // Sort by oldest first
        std::sort(candles.begin(), candles.end(), [](const Candle& a, const Candle& b) {
            return a.date < b.date;
        });

    } catch (const std::exception& e) {
        std::cerr << "Error parsing API response: " << e.what() << std::endl;
        return {};
    }

    return candles;
}

// Read data based on source
std::vector<Candle> DataReader::readData() {
    if (dataSource == "CSV") {
        return readCSV();
    } else if (dataSource == "API") {
        return readAPI();
    } else {
        std::cerr << "Invalid data source: " << dataSource << std::endl;
        return {};
    }
}
