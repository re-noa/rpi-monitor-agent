#include "core/DatabaseManager.hpp"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& dbPath) 
    : m_dbPath(dbPath), m_db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::open() {
    return sqlite3_open(m_dbPath.c_str(), &m_db) == SQLITE_OK;
}

void DatabaseManager::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

bool DatabaseManager::initTable() {
    const char* sql_metrics = "CREATE TABLE IF NOT EXISTS metrics ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                              "cpu_temp REAL,"
                              "ram_usage REAL,"
                              "disk_usage REAL);";
                              
    const char* sql_sites = "CREATE TABLE IF NOT EXISTS site_status ("
                            "url TEXT PRIMARY KEY,"
                            "is_up INTEGER,"
                            "last_check DATETIME DEFAULT CURRENT_TIMESTAMP);";

    const char* sql_index = "CREATE INDEX IF NOT EXISTS idx_metrics_timestamp ON metrics(timestamp);";

    sqlite3_exec(m_db, sql_metrics, nullptr, nullptr, nullptr);
    sqlite3_exec(m_db, sql_index, nullptr, nullptr, nullptr);
    return sqlite3_exec(m_db, sql_sites, nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool DatabaseManager::insertMetrics(double cpu, double ram, double disk) {
    std::string sql = "INSERT INTO metrics (cpu_temp, ram_usage, disk_usage) VALUES (" +
                      std::to_string(cpu) + ", " + std::to_string(ram) + ", " + std::to_string(disk) + ");";
    
    return sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool DatabaseManager::updateSiteStatus(const std::string& url, bool isUp) {
    std::string sql = "INSERT INTO site_status (url, is_up, last_check) VALUES ('" + 
                      url + "', " + (isUp ? "1" : "0") + ", CURRENT_TIMESTAMP) "
                      "ON CONFLICT(url) DO UPDATE SET is_up=excluded.is_up, last_check=CURRENT_TIMESTAMP;";
    return sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool DatabaseManager::purgeOldMetrics(int retentionDays) {
    std::string sql = "DELETE FROM metrics WHERE timestamp < datetime('now', '-" +
                      std::to_string(retentionDays) + " days');";
    return sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool DatabaseManager::pruneSiteStatus(const std::vector<std::string>& monitoredSites) {
    sqlite3_stmt* select = nullptr;
    if (sqlite3_prepare_v2(m_db, "SELECT url FROM site_status;", -1, &select, nullptr) != SQLITE_OK) {
        return false;
    }

    std::vector<std::string> staleUrls;
    while (sqlite3_step(select) == SQLITE_ROW) {
        std::string url = reinterpret_cast<const char*>(sqlite3_column_text(select, 0));
        bool stillMonitored = false;
        for (const auto& site : monitoredSites) {
            if (site == url) {
                stillMonitored = true;
                break;
            }
        }
        if (!stillMonitored) staleUrls.push_back(url);
    }
    sqlite3_finalize(select);

    sqlite3_stmt* remove = nullptr;
    if (sqlite3_prepare_v2(m_db, "DELETE FROM site_status WHERE url = ?;", -1, &remove, nullptr) != SQLITE_OK) {
        return false;
    }

    bool ok = true;
    for (const auto& url : staleUrls) {
        sqlite3_bind_text(remove, 1, url.c_str(), -1, SQLITE_TRANSIENT);
        ok = (sqlite3_step(remove) == SQLITE_DONE) && ok;
        sqlite3_reset(remove);
    }
    sqlite3_finalize(remove);
    return ok;
}
