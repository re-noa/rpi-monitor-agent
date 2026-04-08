#include "core/ConfigManager.hpp"
#include "core/DatabaseManager.hpp"
#include "collector/CpuCollector.hpp"
#include "collector/MemoryCollector.hpp"
#include "collector/DiskCollector.hpp"
#include "collector/WebCollector.hpp"
#include "notifier/DiscordBotNotifier.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <map>

std::string formatDouble(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

int main() {
    ConfigManager config("config.json");
    
    if (!config.load()) {
        std::cerr << "[Error] config.json missing!" << std::endl;
        std::cerr << "[Hint] Copy config.json.template to config.json and fill your secrets." << std::endl;
        return 1;
    }

    DatabaseManager db("metrics.db");
    if (!db.open()) {
        std::cerr << "[Error] Failed to open database: metrics.db" << std::endl;
        return 1;
    }

    if (!db.initTable()) {
        std::cerr << "[Error] Failed to initialize database table." << std::endl;
        return 1;
    }

    CpuCollector cpu;
    MemoryCollector mem;
    DiskCollector disk;
    WebCollector web;
    DiscordBotNotifier notifier(config.getToken(), config.getUserId());

    bool tempAlertSent = false;
    bool ramAlertSent = false;
    bool diskAlertSent = false;
    int ramOverThresholdDuration = 0;
    
    std::map<std::string, int> siteFailCounts;
    const int MAX_FAILURES = 3;

    while (true) {
        double temp = cpu.getTemperature();
        double ram = mem.getMemoryUsage();
        double diskUsage = disk.getDiskUsage();

        db.insertMetrics(temp, ram, diskUsage);

        if (temp > config.getTempThreshold() && !tempAlertSent) {
            if (notifier.sendMessage("Alert: CPU temp at " + formatDouble(temp, 1) + "°C")) {
                tempAlertSent = true;
            }
        } else if (temp < config.getTempThreshold() - 5.0) {
            tempAlertSent = false;
        }

        if (ram > config.getRamThreshold()) {
            ramOverThresholdDuration += config.getInterval();
            if (ramOverThresholdDuration >= config.getRamAlertDelay() && !ramAlertSent) {
                if (notifier.sendMessage("Alert: RAM usage above " + formatDouble(ram, 1) + "% for " + std::to_string(ramOverThresholdDuration) + "s")) {
                    ramAlertSent = true;
                }
            }
        } else {
            ramOverThresholdDuration = 0;
            if (ram < config.getRamThreshold() - 5.0) {
                ramAlertSent = false;
            }
        }

        if (diskUsage > config.getDiskThreshold() && !diskAlertSent) {
            std::string msg = "Alert: Disk usage is high: " + formatDouble(diskUsage, 1) + "%";
            if (notifier.sendMessage(msg)) {
                diskAlertSent = true;
            }
        } else if (diskUsage < config.getDiskThreshold() - 5.0) {
            diskAlertSent = false;
        }

        for (const auto& url : config.getTargetSites()) {
            bool isUp = web.isSiteUp(url);
            
            db.updateSiteStatus(url, isUp);

            if (!isUp) {
                siteFailCounts[url]++;
                
                if (siteFailCounts[url] == MAX_FAILURES) {
                    notifier.sendMessage("Website Down: " + url + " (Confirmed)");
                }
            } 
            else {
                if (siteFailCounts[url] >= MAX_FAILURES) {
                    notifier.sendMessage("Website Back Online: " + url);
                }
                
                siteFailCounts[url] = 0;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.getInterval()));
    }

    return 0;
}
