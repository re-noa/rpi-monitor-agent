#include "core/ConfigManager.hpp"
#include "core/DatabaseManager.hpp"
#include "collector/CpuCollector.hpp"
#include "collector/MemoryCollector.hpp"
#include "collector/DiskCollector.hpp"
#include "collector/WebCollector.hpp"
#include "collector/CertCollector.hpp"
#include "collector/BackupCollector.hpp"
#include "notifier/NtfyNotifier.hpp"
#include "notifier/HeartbeatNotifier.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <sstream>
#include <map>
#include <optional>

// "https://cloud.noa-r.fr/status.php" -> "cloud.noa-r.fr"
std::string displayHost(const std::string& url) {
    std::string host = url;
    auto scheme = host.find("://");
    if (scheme != std::string::npos) host = host.substr(scheme + 3);
    auto path = host.find('/');
    return path == std::string::npos ? host : host.substr(0, path);
}

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

    // Sites removed from config.json would otherwise stay on the dashboard forever
    db.pruneSiteStatus(config.getTargetSites());

    CpuCollector cpu;
    MemoryCollector mem;
    DiskCollector disk;
    WebCollector web;
    CertCollector certs;
    BackupCollector backup;
    NtfyNotifier notifier(config.getNtfyUrl(), config.getNtfyTopic(), config.getNtfyToken());
    HeartbeatNotifier heartbeat(config.getHeartbeatUrl());

    bool tempAlertSent = false;
    bool ramAlertSent = false;
    bool diskAlertSent = false;
    bool backupAlertSent = false;
    int ramOverThresholdDuration = 0;

    std::map<std::string, int> siteFailCounts;
    std::map<std::string, bool> certAlertSent;
    const int MAX_FAILURES = 3;

    // Slow checks (certificates, backup, DB purge) run at startup, then periodically
    using Clock = std::chrono::steady_clock;
    const auto slowInterval = std::chrono::seconds(config.getSlowCheckInterval());
    const auto heartbeatInterval = std::chrono::seconds(config.getHeartbeatInterval());
    const auto purgeInterval = std::chrono::hours(24);
    auto nextSlowCheck = Clock::now();
    auto nextHeartbeat = Clock::now();
    auto nextPurge = Clock::now();

    while (true) {
        double temp = cpu.getTemperature();
        double ram = mem.getMemoryUsage();
        double diskUsage = disk.getDiskUsage();

        db.insertMetrics(temp, ram, diskUsage);

        if (temp > config.getTempThreshold() && !tempAlertSent) {
            if (notifier.sendMessage("Température élevée", "CPU à " + formatDouble(temp, 1) + " °C.")) {
                tempAlertSent = true;
            }
        } else if (temp < config.getTempThreshold() - 5.0) {
            tempAlertSent = false;
        }

        if (ram > config.getRamThreshold()) {
            ramOverThresholdDuration += config.getInterval();
            if (ramOverThresholdDuration >= config.getRamAlertDelay() && !ramAlertSent) {
                if (notifier.sendMessage("Mémoire saturée", "RAM à " + formatDouble(ram, 1) + " % depuis " + std::to_string(ramOverThresholdDuration) + " s.")) {
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
            if (notifier.sendMessage("Disque presque plein", "Disque utilisé à " + formatDouble(diskUsage, 1) + " %.")) {
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
                    notifier.sendMessage("Site indisponible", displayHost(url) + " ne répond plus.");
                }
            } 
            else {
                if (siteFailCounts[url] >= MAX_FAILURES) {
                    notifier.sendMessage("Site rétabli", displayHost(url) + " répond à nouveau.", "default");
                }
                
                siteFailCounts[url] = 0;
            }
        }

        if (Clock::now() >= nextSlowCheck) {
            nextSlowCheck = Clock::now() + slowInterval;

            for (const auto& url : config.getTargetSites()) {
                std::optional<int> expiry = certs.getDaysUntilExpiry(url);
                if (!expiry) continue;
                int days = *expiry;

                if (days <= config.getCertWarningDays() && !certAlertSent[url]) {
                    if (notifier.sendMessage("Certificat HTTPS", displayHost(url) + (days < 0 ? " : certificat expiré." : " : expire dans " + std::to_string(days) + " jours."))) {
                        certAlertSent[url] = true;
                    }
                } else if (days > config.getCertWarningDays()) {
                    certAlertSent[url] = false;
                }
            }

            if (!config.getBackupStatusFile().empty()) {
                double hours = backup.getHoursSinceLastBackup(config.getBackupStatusFile());
                bool backupStale = hours < 0 || hours > config.getBackupMaxAgeHours();

                if (backupStale && !backupAlertSent) {
                    std::string msg = hours < 0
                        ? "Aucune sauvegarde réussie trouvée."
                        : "Dernière sauvegarde réussie il y a " + formatDouble(hours, 0) + " h.";
                    if (notifier.sendMessage("Sauvegarde en retard", msg)) {
                        backupAlertSent = true;
                    }
                } else if (!backupStale && backupAlertSent) {
                    notifier.sendMessage("Sauvegarde rétablie", "Dernière sauvegarde réussie il y a " + formatDouble(hours, 0) + " h.", "default");
                    backupAlertSent = false;
                }
            }
        }

        if (Clock::now() >= nextPurge) {
            nextPurge = Clock::now() + purgeInterval;
            db.purgeOldMetrics(config.getMetricsRetentionDays());
        }

        if (heartbeat.isEnabled() && Clock::now() >= nextHeartbeat) {
            nextHeartbeat = Clock::now() + heartbeatInterval;
            heartbeat.ping();
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.getInterval()));
    }

    return 0;
}
