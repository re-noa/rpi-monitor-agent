#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    ConfigManager(const std::string& filename);
    bool load();

    double getTempThreshold() const;
    int getInterval() const;

private:
    std::string m_filename;
    nlohmann::json m_data;
};

#endif
