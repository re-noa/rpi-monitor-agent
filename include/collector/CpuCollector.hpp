#ifndef CPU_COLLECTOR_HPP
#define CPU_COLLECTOR_HPP

#include <string>

class CpuCollector {
public:
    CpuCollector();
    double getTemperature();

private:
    const std::string m_thermalPath = "/sys/class/thermal/thermal_zone0/temp";
};

#endif
