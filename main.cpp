#include "collector/CpuCollector.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    CpuCollector cpu;

    while (true) {
        double temp = cpu.getTemperature();
        if (temp >= 0) {
            std::cout << "[Monitor] CPU Temp: " << temp << " °C" << std::endl;
        } else {
            std::cerr << "[Error] Failed to read sensor" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
