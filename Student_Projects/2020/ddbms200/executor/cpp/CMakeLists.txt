cmake_minimum_required(VERSION 3.5.1)
#project(main)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}"-pthread -std=c++11")

execute_process(COMMAND mysql_config --cflags OUTPUT_VARIABLE MYSQL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND mysql_config --libs OUTPUT_VARIABLE MYSQL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_CXX_COMPILER ${MYSQL_CFLAGS})

#生成链接库
add_library(Mysql_connector mysql_connector.cpp)
target_link_libraries(Mysql_connector ${MYSQL_LIBS})

add_library(Executor executor.cpp)
target_link_libraries(Executor Client Mysql_connector)
target_include_directories(Executor PUBLIC "../../transfer/transfer")

add_library(ExeMulti executor_multi.cpp executor.h)
target_link_libraries(ExeMulti Executor)
