cmake_minimum_required(VERSION 3.15)
project(RP_CONTROL CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(RP_CONTROL main.cpp)

find_package(libssh REQUIRED)

target_link_libraries(RP_CONTROL ssh)