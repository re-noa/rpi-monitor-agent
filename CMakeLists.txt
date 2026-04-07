cmake_minimum_required(VERSION 3.10)
project(rpi_monitor_agent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(monitor_agent main.cpp)
