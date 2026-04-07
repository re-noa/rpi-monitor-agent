cmake_minimum_required(VERSION 3.10)
project(RPiMonitorAgent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(monitor_agent 
    src/main.cpp 
    src/core/ConfigManager.cpp
    src/collector/CpuCollector.cpp
    src/collector/MemoryCollector.cpp
)

target_link_libraries(monitor_agent PRIVATE nlohmann_json::nlohmann_json)
target_compile_options(monitor_agent PRIVATE -Wall -Wextra)
