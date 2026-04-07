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
    const char* sql = "CREATE TABLE IF NOT EXISTS metrics ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                      "cpu_temp REAL,"
                      "ram_usage REAL,"
                      "disk_usage REAL);";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::insertMetrics(double cpu, double ram, double disk) {
    std::string sql = "INSERT INTO metrics (cpu_temp, ram_usage, disk_usage) VALUES (" +
                      std::to_string(cpu) + ", " + std::to_string(ram) + ", " + std::to_string(disk) + ");";
    
    return sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}
