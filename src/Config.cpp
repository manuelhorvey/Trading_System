#include "Config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

using json = nlohmann::json;

Config Config::load(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not open config file: " + filename);

    json configJson;
    file >> configJson;

    for (const auto &field : {"csv_path", "data_source", "api_endpoint", "api_key"})
    {
        if (!configJson.contains(field))
            throw std::runtime_error(std::string("Missing config field: ") + field);
    }

    return Config{
        configJson["csv_path"],
        configJson["data_source"],
        configJson["api_endpoint"],
        configJson["api_key"]
    };
}
