#include <spdlog/spdlog.h>
#include "Config.h"
#include "OrderBlockAnalyzer.h"
#include <thread>
#include <chrono>
#include <csignal>
#include <atomic>

std::atomic<bool> keepRunning(true);

void signalHandler(int)
{
    spdlog::info("Signal received, shutting down...");
    keepRunning = false;
}

int main()
{
    spdlog::set_pattern("%^%l%$ [%Y-%m-%d %H:%M:%S] %v");
    spdlog::set_level(spdlog::level::info);

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    Config config;
    try
    {
        config = Config::load("../config/settings.json");
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to load config: {}", e.what());
        return 1;
    }

    spdlog::info("Trading system started with config loaded.");

    OrderBlockAnalyzer analyzer(config);

    while (keepRunning)
    {
        try
        {
            analyzer.analyze();
        }
        catch (const std::exception &e)
        {
            spdlog::error("Exception during analysis: {}", e.what());
        }
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }

    spdlog::info("Trading system exited cleanly.");
    return 0;
}
