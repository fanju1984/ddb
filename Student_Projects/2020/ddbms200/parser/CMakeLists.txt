cmake_minimum_required(VERSION 3.10)
project(DDBMS)

add_executable(main Parse_LOAD.cpp Tools.cpp)
target_link_libraries(main PUBLIC Executor)
target_include_directories(main PUBLIC "../executor/cpp")
target_link_libraries(main PUBLIC Metadata)
target_include_directories(main PUBLIC "../metadata")