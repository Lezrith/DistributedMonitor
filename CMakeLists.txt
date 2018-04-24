cmake_minimum_required(VERSION 3.5)
project(DistributedMonitor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pthread")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-g" CACHE INTERNAL "" FORCE)
find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

include_directories(include include/Core include/Messages include/Serializers include/Utils)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

configure_file(peers.config peers.config COPYONLY)

add_executable(DistributedMonitor ${SOURCES} ${HEADERS})

target_include_directories(DistributedMonitor PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(DistributedMonitor PUBLIC ${ZeroMQ_LIBRARY})
