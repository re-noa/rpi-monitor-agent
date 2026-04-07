#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    ConfigManager(const std::string& filename);
    bool load();

    std::string getToken() const;
    std::string getUserId() const;
    double getTempThreshold() const;
    double getRamThreshold() const;
    int getRamAlertDelay() const;
    int getInterval() const;

private:
    std::string m_filename;
    nlohmann::json m_data;
};

#endif
