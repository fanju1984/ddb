cmake_minimum_required(VERSION 2.9.10)
project(network-test C CXX)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# network-utils
set(CMAKE_PREFIX_PATH ../../third-party)
find_library(NETWORK_LIB NAMES network-utils)



if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_executable(network-test network-test.cpp network.h)

target_link_libraries(network-test ${NETWORK_LIB})

