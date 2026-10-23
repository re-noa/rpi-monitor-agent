#ifndef BACKUP_COLLECTOR_HPP
#define BACKUP_COLLECTOR_HPP

#include <string>

class BackupCollector {
public:
    BackupCollector();
    // Hours since the last successful backup, or -1 if the status file is missing
    double getHoursSinceLastBackup(const std::string& statusFile);
};

#endif
