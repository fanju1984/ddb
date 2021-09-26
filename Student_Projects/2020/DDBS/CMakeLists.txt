cmake_minimum_required(VERSION 2.9.10)
set(CMAKE_CXX_STANDARD 11)
project(main C CXX)

# 添加 sql-processor 子目录
add_subdirectory(sql-processor)

# 添加 data-loader 子目录
add_subdirectory(data-loader)

add_subdirectory(query-tree-scheduler)

add_subdirectory(network-utils)

# network-utils
# set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR}/query-tree-scheduler)
# find_library(NETWORK_LIB NAMES network-utils)

add_executable(main main.cpp)

target_link_libraries(main SQLProcessor)
target_link_libraries(main DataLoader)
target_link_libraries(main network-utils)

# add boost/regex
set(BOOST_ROOT /usr/local/boost)
set(Boost_NO_SYSTEM_PATHS ON)
find_package(Boost COMPONENTS regex REQUIRED)
if(Boost_FOUND)
    MESSAGE(STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}")
    MESSAGE(STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(main ${Boost_LIBRARIES})
endif()