include(ExternalProject)

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.8)

# Project's name
project(autobahn)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Set C++11 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall -Wextra") 
set(CMAKE_CXX_FLAGS_RELEASE "-msse4 -O2 -Wall -Wextra -flto -ffunction-sections -fdata-sections") 
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Source files
file(GLOB SOURCES "autobahn/src/*.cpp")

#set(Boost_INCLUDE_DIRS "third_party/boost_1_68_0")
#set(Boost_LIBRARY_DIRS "third_party/boost_1_68_0/libs")
#find_package(Boost 1.68 REQUIRED COMPONENTS algorithm asio process)
include(boost.cmake)
include_directories(${Boost_INCLUDE_DIR})
#include_directories(${MSGPACK_INCLUDE_DIR})
#include_directories(${ZeroMQ_INCLUDE_DIR})
#include_directories(${CppZMQ_INCLUDE_DIR})
#include_directories(${NATS_INCLUDE_DIR})

# msgpack dependency
ExternalProject_Add(msgpack
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party/msgpack
    GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
    GIT_TAG "cpp-3.2.0"
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                  -DMSGPACK_ENABLE_CXX=OFF
                  -DMSGPACK_BUILD_EXAMPLES=OFF
                  -DMSGPACK_CXX17=ON
    INSTALL_COMMAND ""
)
set(MSGPACK_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/msgpack/src/msgpack/include)
include_directories(${MSGPACK_INCLUDE_DIR})
add_library(libmsgpack STATIC IMPORTED)
add_dependencies(libmsgpack msgpack)
set_property(TARGET libmsgpack PROPERTY IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/third_party/msgpack/src/msgpack-build/libmsgpackc.a)

# zeromq dependency
ExternalProject_Add(zeromq
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party/zeromq
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG "v4.3.2"
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                  -DZMQ_BUILD_TESTS=OFF
    INSTALL_COMMAND ""
)
set(ZEROMQ_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/zeromq/include)
include_directories(${ZEROMQ_INCLUDE_DIR})
add_library(libzmq STATIC IMPORTED)
add_dependencies(libzmq zeromq)
set_property(TARGET libzmq PROPERTY IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/third_party/zeromq/src/zeromq-build/lib/libzmq.a)

# cppzmq dependency
ExternalProject_Add(cppzmq
    DEPENDS libzmq
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party/cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    GIT_TAG "v4.5.0"
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                  -DCPPZMQ_BUILD_TESTS=OFF
    INSTALL_COMMAND ""
)
set(CPPZMQ_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/cppzmq/src/cppzmq)
include_directories(${CPPZMQ_INCLUDE_DIR})
#add_library(libcppzmq STATIC IMPORTED)
#add_dependencies(libcppzmq cppzmq)

# nats dependency
ExternalProject_Add(nats
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party/nats
    GIT_REPOSITORY https://github.com/nats-io/nats.c.git
    GIT_TAG "v2.1.0"
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
                  -DNATS_BUILD_STREAMING=OFF
    INSTALL_COMMAND ""
)
set(NATS_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/third_party/nats/src/nats/src)
include_directories(${NATS_INCLUDE_DIR})
add_library(libnats STATIC IMPORTED)
add_dependencies(libnats nats)
set_property(TARGET libnats PROPERTY IMPORTED_LOCATION  ${CMAKE_CURRENT_BINARY_DIR}/third_party/nats/src/nats-build/src/libnats_static.a)

# Build autobahn-server
add_executable(autobahn-server "autobahn/src/autobahn-server.cpp")
# add_dependencies(autobahn-server zeromq cppzmq nats)
target_link_libraries(autobahn-server libmsgpack libzmq libnats pthread crypto ssl ${Boost_LIBRARIES})
