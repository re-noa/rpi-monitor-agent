#include "collector/CpuCollector.hpp"
#include "collector/MemoryCollector.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

int main() {
    CpuCollector cpu;
    MemoryCollector mem;

    std::cout << "--- RPi Monitor Agent Started ---" << std::endl;
    std::cout << std::fixed << std::setprecision(1);

    while (true) {
        double temp = cpu.getTemperature();
        double ramUsage = mem.getMemoryUsage();
        
        std::cout << "[Monitor] "
                  << "CPU: " << temp << " °C | "
                  << "RAM: " << ramUsage << "%" 
                  << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
