cmake_minimum_required(VERSION 3.12)
project(FunctionalBadges)

find_package(LibLogicalAccess NO_MODULE REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${LIBLOGICALACCESS_INCLUDE_DIRS})

add_executable(FunctionalBadges main.cpp)
target_link_libraries(FunctionalBadges ${LIBLOGICALACCESS_LIBRARIES})