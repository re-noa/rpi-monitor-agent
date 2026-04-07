#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <string>
#include <sqlite3.h>

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool open();
    void close();
    bool initTable();
    bool insertMetrics(double cpu, double ram, double disk);

private:
    std::string m_dbPath;
    sqlite3* m_db;
};

#endif
