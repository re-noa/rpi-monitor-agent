#include "collector/MemoryCollector.hpp"
#include <fstream>
#include <string>
#include <algorithm>

MemoryCollector::MemoryCollector() {}

long MemoryCollector::extractValue(const std::string& line) {
    std::string digits = "";
    for (char c : line) {
        if (std::isdigit(c)) {
            digits += c;
        }
    }
    return digits.empty() ? 0 : std::stol(digits);
}

double MemoryCollector::getMemoryUsage() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return -1.0;

    std::string line;
    long total = 0, available = 0;

    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) {
            total = extractValue(line);
        } else if (line.find("MemAvailable:") == 0) {
            available = extractValue(line);
        }
        
        if (total > 0 && available > 0) break;
    }

    return (total == 0) ? -1.0 : (1.0 - (double)available / total) * 100.0;
}
