#ifndef MEMORY_COLLECTOR_HPP
#define MEMORY_COLLECTOR_HPP

#include <string>

class MemoryCollector {
public:
    MemoryCollector();
    double getMemoryUsage();

private:
    const std::string m_memInfoPath = "/proc/meminfo";
    long extractValue(const std::string& line);
};

#endif
