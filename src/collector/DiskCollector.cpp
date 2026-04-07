#include "collector/DiskCollector.hpp"
#include <sys/statvfs.h>

DiskCollector::DiskCollector() {}

double DiskCollector::getDiskUsage() {
    struct statvfs stats;
    
    if (statvfs(m_path.c_str(), &stats) != 0) {
        return -1.0;
    }

    unsigned long total_blocks = stats.f_blocks;
    unsigned long free_blocks = stats.f_bavail;

    if (total_blocks == 0) return -1.0;

    double used_pct = 100.0 * (1.0 - (double)free_blocks / total_blocks);
    return used_pct;
}
