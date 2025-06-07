cmake_minimum_required(VERSION 3.10)

project(TradingSystem VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Correctly specify the path to spdlog include directory
include_directories(${PROJECT_SOURCE_DIR}/spdlog/include)

# Find the CURL package
find_package(CURL REQUIRED)

# Include your own header files
include_directories(include)

# Add the executable
add_executable(TradingSystem
    src/main.cpp
    src/DataReader.cpp
    src/OrderBlock.cpp
    src/MarketStructure.cpp
    src/Backtest.cpp
    src/Strategy.cpp
    src/Utils.cpp
    src/Order.cpp
    # Remove these lines until files are created
    src/Config.cpp
    src/OrderBlockAnalyzer.cpp
    src/TradingUtils.cpp
    src/StructureUtils.cpp
    src/LoggingUtils.cpp
)


# Link the CURL library
target_link_libraries(TradingSystem CURL::libcurl)
