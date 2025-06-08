# TradingSystem

A modular and extensible C++ trading system designed to automate and manage trading strategies across multiple markets and assets. The system supports backtesting, real-time trading, risk management, and logging.

## Features

- **Strategy Management:** Easily add, remove, or modify trading strategies via the `Strategy` interface.
- **Market Integration:** Connects to various data sources (CSV, API) for historical and live data.
- **Order Execution:** Automated order placement and management.
- **Risk Management:** Configurable risk controls and position sizing.
- **Backtesting:** Simulate strategies using historical data.
- **Logging & Monitoring:** Real-time performance tracking and error logging using [spdlog](spdlog/README.md).

## Installation

### Prerequisites

- C++11 or newer compiler
- [CMake](https://cmake.org/) >= 3.10
- [spdlog](https://github.com/gabime/spdlog) (included as a submodule or in `spdlog/`)
- [nlohmann/json](https://github.com/nlohmann/json) (included as `include/json.hpp`)
- [libcurl](https://curl.se/libcurl/) (for API data fetching)

### Build

```bash
git clone https://github.com/manuelhorvey/TradingSystem.git
cd TradingSystem
mkdir build && cd build
cmake ..
make
```

The main executable will be built in the `build/` directory.

## Usage

1. Configure your strategies and exchange credentials in `config/settings.json`.
2. Place your historical data files in the `data/` directory.
3. Run the trading system:

    ```bash
    ./TradingSystem
    ```

4. Monitor logs and performance metrics in the `logs/` directory (created at runtime).

## Project Structure

```
TradingSystem/
├── include/         # C++ headers (core classes: Strategy, Backtest, DataReader, etc.)
├── src/             # C++ source files (main logic)
├── data/            # Historical market data (CSV files)
├── config/          # Configuration files (settings.json)
├── spdlog/          # Logging library
├── models/          # Model definitions (if any)
├── scripts/         # Utility scripts (if any)
├── tests/           # Unit and integration tests
├── build/           # Build output (created after compilation)
├── CMakeLists.txt   # Build configuration
├── README.md        # Project documentation
└── main.cpp         # Entry point (if present)
```

## Configuration

- **Strategy parameters, API keys, and other settings** are managed in `config/settings.json`.
- **Historical data** should be placed in the `data/` directory as CSV files.

## Logging

- Logging is handled by [spdlog](spdlog/README.md).
- Log files are written to the `logs/` directory.

## Contributing

Contributions are welcome! Please open issues or submit pull requests for improvements.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.