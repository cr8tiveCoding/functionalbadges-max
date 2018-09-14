cmake_minimum_required(VERSION 3.12)
project(FunctionalBadges)

find_package(LibLogicalAccess NO_MODULE REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${LIBLOGICALACCESS_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g -llogicalaccess -DUNIX")

add_executable(FunctionalBadges main.cpp Functions.h)
link_directories(plugins)
target_link_libraries(FunctionalBadges ${LIBLOGICALACCESS_LIBRARIES} libmifarecards.so)