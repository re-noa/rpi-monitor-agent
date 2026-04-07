#include "collector/CpuCollector.hpp"
#include <fstream>
#include <string>

CpuCollector::CpuCollector() {}

double CpuCollector::getTemperature() {
    std::ifstream file(m_thermalPath);
    if (!file.is_open()) {
        return -1.0;
    }

    std::string raw;
    std::getline(file, raw);
    file.close();

    try {
        return std::stod(raw) / 1000.0;
    } catch (...) {
        return -1.0;
    }
}
