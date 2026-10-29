#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <string>
#include <vector>
#include <sqlite3.h>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool open();
    void close();
    bool initTable();
    bool insertMetrics(double cpu, double ram, double disk);
    bool updateSiteStatus(const std::string& url, bool isUp);
    bool purgeOldMetrics(int retentionDays);
    // Removes status rows of sites no longer listed in the config
    bool pruneSiteStatus(const std::vector<std::string>& monitoredSites);

private:
    std::string m_dbPath;
    sqlite3* m_db;
};

#endif
