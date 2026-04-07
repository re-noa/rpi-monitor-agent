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

    sqlite3_exec(m_db, sql_metrics, nullptr, nullptr, nullptr);
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
