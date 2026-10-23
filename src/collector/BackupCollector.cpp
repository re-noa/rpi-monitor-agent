#include "collector/BackupCollector.hpp"
#include <sys/stat.h>
#include <ctime>

BackupCollector::BackupCollector() {}

double BackupCollector::getHoursSinceLastBackup(const std::string& statusFile) {
    struct stat info;

    // The backup script touches this file after each successful run
    if (statusFile.empty() || stat(statusFile.c_str(), &info) != 0) {
        return -1.0;
    }

    return std::difftime(std::time(nullptr), info.st_mtime) / 3600.0;
}
