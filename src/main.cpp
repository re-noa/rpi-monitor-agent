#include "core/ConfigManager.hpp"
#include "collector/CpuCollector.hpp"
#include "collector/MemoryCollector.hpp"
#include "notifier/DiscordBotNotifier.hpp"
#include <thread>
#include <chrono>

int main() {
    ConfigManager config("config.json");
    if (!config.load()) return 1;

    CpuCollector cpu;
    MemoryCollector mem;
    DiscordBotNotifier notifier(config.getToken(), config.getUserId());

    bool tempAlertSent = false;
    bool ramAlertSent = false;
    int ramOverThresholdDuration = 0;

    while (true) {
        double temp = cpu.getTemperature();
        double ram = mem.getMemoryUsage();

        if (temp > config.getTempThreshold() && !tempAlertSent) {
            if (notifier.sendMessage("Alert: CPU temp at " + std::to_string(temp) + "C")) {
                tempAlertSent = true;
            }
        } else if (temp < config.getTempThreshold() - 5.0) {
            tempAlertSent = false;
        }

        if (ram > config.getRamThreshold()) {
            ramOverThresholdDuration += config.getInterval();
            if (ramOverThresholdDuration >= config.getRamAlertDelay() && !ramAlertSent) {
                if (notifier.sendMessage("Alert: RAM usage above " + std::to_string(ram) + "% for " + std::to_string(ramOverThresholdDuration) + "s")) {
                    ramAlertSent = true;
                }
            }
        } else {
            ramOverThresholdDuration = 0;
            if (ram < config.getRamThreshold() - 5.0) {
                ramAlertSent = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.getInterval()));
    }

    return 0;
}
