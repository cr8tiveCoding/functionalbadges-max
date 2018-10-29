cmake_minimum_required(VERSION 3.12)
project(FunctionalBadges)

find_package(LibLogicalAccess NO_MODULE REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(${LIBLOGICALACCESS_INCLUDE_DIRS})
include_directories(/usr/local/include/pistache)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -O1 -fno-omit-frame-pointer -g -llogicalaccess -DUNIX")

add_executable(FunctionalBadgeWriter writer_main.cpp functions.h macro.h data.h)
add_executable(FunctionalBadgeReader reader_main.cpp functions.h macro.h data.h web.h)

link_directories(plugins)
target_link_libraries(FunctionalBadgeWriter ${LIBLOGICALACCESS_LIBRARIES} libmifarecards.so)
target_link_libraries(FunctionalBadgeReader ${LIBLOGICALACCESS_LIBRARIES} libmifarecards.so)
target_link_libraries(FunctionalBadgeReader /usr/local/lib/libpistache.a)