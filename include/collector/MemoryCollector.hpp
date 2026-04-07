#ifndef MEMORY_COLLECTOR_HPP
#define MEMORY_COLLECTOR_HPP

#include <string>

class MemoryCollector {
public:
    MemoryCollector();
    double getMemoryUsage();

private:
    long extractValue(const std::string& line);
};

#endif
