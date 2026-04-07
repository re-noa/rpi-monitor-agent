#ifndef DISK_COLLECTOR_HPP
#define DISK_COLLECTOR_HPP

#include <string>

class DiskCollector {
public:
    DiskCollector();
    double getDiskUsage();
private:
    const std::string m_path = "/";
};

#endif
