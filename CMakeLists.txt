cmake_minimum_required(VERSION 3.12)
project(FunctionalBadges)

find_package(LibLogicalAccess NO_MODULE REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(FunctionalBadges main.cpp)