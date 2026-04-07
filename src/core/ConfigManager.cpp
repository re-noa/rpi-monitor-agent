#include "core/ConfigManager.hpp"
#include <fstream>

ConfigManager::ConfigManager(const std::string& filename) : m_filename(filename) {}

bool ConfigManager::load() {
    std::ifstream file(m_filename);
    if (!file.is_open()) return false;
    try {
        file >> m_data;
        return true;
    } catch (...) {
        return false;
    }
}

std::string ConfigManager::getToken() const {
    return m_data.value("discord_token", "");
}

std::string ConfigManager::getUserId() const {
    return m_data.value("discord_user_id", "");
}

double ConfigManager::getTempThreshold() const {
    return m_data.value("temp_threshold", 60.0);
}

double ConfigManager::getRamThreshold() const {
    return m_data.value("ram_threshold", 95.0);
}

int ConfigManager::getRamAlertDelay() const {
    return m_data.value("ram_alert_delay", 60);
}

int ConfigManager::getInterval() const {
    return m_data.value("interval_seconds", 5);
}

double ConfigManager::getDiskThreshold() const {
    return m_data.value("disk_threshold", 90.0);
}

std::vector<std::string> ConfigManager::getTargetSites() const {
    return m_data.value("sites_to_monitor", std::vector<std::string>{});
}
