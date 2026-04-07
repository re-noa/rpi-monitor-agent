#include "core/ConfigManager.hpp"
#include "collector/CpuCollector.hpp"
#include "collector/MemoryCollector.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

int main() {
    ConfigManager config("config.json");
    if (!config.load()) {
        std::cerr << "[Error] Failed to load config.json" << std::endl;
        return 1;
    }

    CpuCollector cpu;
    MemoryCollector mem;

    std::cout << "--- RPi Monitor Agent (Core Loaded) ---" << std::endl;
    std::cout << "Threshold: " << config.getTempThreshold() << " °C" << std::endl;
    std::cout << std::fixed << std::setprecision(1);

    while (true) {
        double temp = cpu.getTemperature();
        double ram = mem.getMemoryUsage();

        std::cout << "[Monitor] CPU: " << temp << " °C | RAM: " << ram << " %" << std::endl;

        if (temp > config.getTempThreshold()) {
            std::cout << "  >> ALERT: High temperature detected!" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.getInterval()));
    }

    return 0;
}
