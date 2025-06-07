#pragma once
#include <string>

struct Config
{
    std::string csv_path;
    std::string data_source;
    std::string api_endpoint;
    std::string api_key;

    static Config load(const std::string &filename);
};
