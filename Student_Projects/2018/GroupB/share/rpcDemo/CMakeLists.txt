project(rpcDemo)
cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread ")
#add_definitions(-std=c++11)
include_directories(/home/tommenx/share/rpclib/include)
add_executable(server server.cpp)
target_link_libraries(server /home/tommenx/share/rpclib/build/librpc.a)

