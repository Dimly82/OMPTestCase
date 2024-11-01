cmake_minimum_required(VERSION 3.14)

project(Manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(sdbus-c++ REQUIRED)

add_executable(Manager main.cc)
target_link_libraries(Manager PRIVATE sdbus-c++)