cmake_minimum_required(VERSION 3.10)
project(RPiMonitorAgent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(monitor_agent 
    main.cpp 
    src/collector/CpuCollector.cpp
)

target_compile_options(monitor_agent PRIVATE -Wall -Wextra)
