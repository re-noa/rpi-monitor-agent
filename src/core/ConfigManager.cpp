#include "core/ConfigManager.hpp"
#include <fstream>
#include <iostream>

ConfigManager::ConfigManager(const std::string& filename) : m_filename(filename) {}

bool ConfigManager::load() {
    std::ifstream file(m_filename);
    if (!file.is_open()) {
        return false;
    }

    try {
        file >> m_data;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

double ConfigManager::getTempThreshold() const {
    return m_data.value("temp_threshold", 60.0);
}

int ConfigManager::getInterval() const {
    return m_data.value("interval_seconds", 5);
}
