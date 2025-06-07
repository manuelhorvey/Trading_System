#include "DataReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <iomanip>
#include <ctime>
#include "json.hpp"
#include "Candle.h"

using json = nlohmann::json;

DataReader::DataReader(const std::string& filepath, const std::string& dataSource, const std::string& apiEndpoint, const std::string& apiKey)
    : filepath(filepath), dataSource(dataSource), apiEndpoint(apiEndpoint), apiKey(apiKey) {}

// Helper function to trim leading/trailing whitespace
std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) start++;
    auto end = str.end();
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

// Read candles from CSV file
std::vector<Candle> DataReader::readCSV() {
    std::vector<Candle> candles;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return candles;
    }

    std::string line;
    std::vector<std::string> lines;

    std::getline(file, line);  // skip header
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

        Candle candle;
        candle.date = trim(date);
        try {
            candle.open = std::stod(trim(openStr));
            candle.high = std::stod(trim(highStr));
            candle.low = std::stod(trim(lowStr));
            candle.close = std::stod(trim(closeStr));
            candle.changePercent = std::stod(trim(percentChangeStr));
        } catch (const std::exception& e) {
            std::cerr << "Conversion error on line: " << rawLine << "\nReason: " << e.what() << std::endl;
            continue;
        }

        candles.push_back(candle);
    }

    std::cout << "Loaded " << candles.size() << " candles from CSV.\n";
    return candles;
}

// CURL callback
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Read candles from Binance API
std::vector<Candle> DataReader::readAPI() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<Candle> candles;

    curl = curl_easy_init();
    if (curl) {
        std::string fullUrl = apiEndpoint;

        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0"); // Binance sometimes requires a User-Agent

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "CURL failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return {};
        }

        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        curl_easy_cleanup(curl);

        if (httpCode != 200) {
            std::cerr << "API call failed with HTTP code: " << httpCode << std::endl;
            return {};
        }
    } else {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return {};
    }

    // Parse Binance response
    try {
        json j = json::parse(readBuffer);
        for (const auto& entry : j) {
            if (entry.size() < 6) continue;

            Candle candle;

            // Convert timestamp to readable date
            std::time_t ts = entry[0].get<long long>() / 1000;
            std::stringstream ss;
            ss << std::put_time(std::gmtime(&ts), "%Y-%m-%d %H:%M:%S");
            candle.date = ss.str();

            candle.open = std::stod(entry[1].get<std::string>());
            candle.high = std::stod(entry[2].get<std::string>());
            candle.low = std::stod(entry[3].get<std::string>());
            candle.close = std::stod(entry[4].get<std::string>());

            candle.changePercent = ((candle.close - candle.open) / candle.open) * 100.0;

            candles.push_back(candle);
        }

        std::sort(candles.begin(), candles.end(), [](const Candle& a, const Candle& b) {
            return a.date < b.date;
        });

    } catch (const std::exception& e) {
        std::cerr << "Error parsing Binance API response: " << e.what() << std::endl;
        return {};
    }

    return candles;
}

// Wrapper to pick source
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
