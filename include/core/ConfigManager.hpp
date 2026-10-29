#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <nlohmann/json.hpp>

class ConfigManager {
public:
    ConfigManager(const std::string& filename);
    bool load();

    std::string getNtfyUrl() const;
    std::string getNtfyTopic() const;
    std::string getNtfyToken() const;
    double getTempThreshold() const;
    double getRamThreshold() const;
    int getRamAlertDelay() const;
    int getInterval() const;
    double getDiskThreshold() const;
    std::vector<std::string> getTargetSites() const;
    int getCertWarningDays() const;
    std::string getBackupStatusFile() const;
    double getBackupMaxAgeHours() const;
    int getMetricsRetentionDays() const;
    int getSlowCheckInterval() const;
    std::string getHeartbeatUrl() const;
    int getHeartbeatInterval() const;

private:
    std::string m_filename;
    nlohmann::json m_data;
};

#endif
