cmake_minimum_required(VERSION 3.10)

project(RPiMonitorAgent 
        VERSION 1.0.0 
        DESCRIPTION "Agent de monitoring low-level pour Raspberry Pi"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(monitor_agent 
    src/main.cpp 
    src/core/ConfigManager.cpp
    src/core/DatabaseManager.cpp
    src/collector/CpuCollector.cpp
    src/collector/DiskCollector.cpp
    src/collector/MemoryCollector.cpp
    src/collector/WebCollector.cpp
    src/notifier/DiscordBotNotifier.cpp
)

target_link_libraries(monitor_agent 
    PRIVATE 
    CURL::libcurl 
    nlohmann_json::nlohmann_json
    ${SQLITE3_LIBRARIES}
)

target_compile_options(monitor_agent PRIVATE 
    -Wall 
    -Wextra 
    -Wpedantic 
    -Wshadow 
    -Wnon-virtual-dtor 
    -O3
)
