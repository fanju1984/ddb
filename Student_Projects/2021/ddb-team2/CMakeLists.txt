cmake_minimum_required(VERSION 2.9.10)
project(plan-scheduler C CXX)

include(FindThreads)
include(FindProtobuf)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER plan-scheduler/proto/ddb.proto)

set(CMAKE_C_COMPILER "/opt/rh/devtoolset-7/root/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/opt/rh/devtoolset-7/root/usr/bin/g++")



include_directories(${CMAKE_CURRENT_BINARY_DIR})

# inner include
include_directories(include)

# brpc
set(CMAKE_PREFIX_PATH ~/libs/brpc/)
find_library(BRPC_LIB NAMES libbrpc.a brpc)
include_directories(~/libs/brpc/src/)

# mysql-connector
set(CMAKE_PREFIX_PATH ~/libs/mysql-connector/lib/)
find_library(MYSQL_LIB NAMES mysqlcppconn)
include_directories(~/libs/mysql-connector/include)

# gflags
find_path(GFLAGS_INCLUDE_PATH gflags/gflags.h)
find_library(GFLAGS_LIBRARY NAMES gflags libgflags)
if((NOT GFLAGS_INCLUDE_PATH) OR (NOT GFLAGS_LIBRARY))
    message(FATAL_ERROR "Fail to find gflags")
endif()
include_directories(${GFLAGS_INCLUDE_PATH})



execute_process(
    COMMAND bash -c "grep \"namespace [_A-Za-z0-9]\\+ {\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $2}' | tr -d '\n'"
    OUTPUT_VARIABLE GFLAGS_NS
)
if(${GFLAGS_NS} STREQUAL "GFLAGS_NAMESPACE")
    execute_process(
        COMMAND bash -c "grep \"#define GFLAGS_NAMESPACE [_A-Za-z0-9]\\+\" ${GFLAGS_INCLUDE_PATH}/gflags/gflags_declare.h | head -1 | awk '{print $3}' | tr -d '\n'"
        OUTPUT_VARIABLE GFLAGS_NS
    )
endif()

set(CMAKE_CPP_FLAGS "${DEFINE_CLOCK_GETTIME} -DGFLAGS_NS=${GFLAGS_NS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -std=c++14 -lstdc++ -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

# if(CMAKE_VERSION VERSION_LESS "3.1.3")
#     if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lstdc++")
#     endif()
#     if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lstdc++")
#     endif()
# else()
#     set(CMAKE_CXX_STANDARD 11)
#     set(CMAKE_CXX_STANDARD_REQUIRED ON)
# endif()

# leveldb
find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
include_directories(${LEVELDB_INCLUDE_PATH})

include_directories(${OPENSSL_INCLUDE_DIR})
find_package(OpenSSL)

# include_directories(utils/)
# add_subdirectory(utils)

# include_directories(plan-scheduler/)
# add_subdirectory(plan-scheduler)

set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    dl
    )

include_directories(utils/)
include_directories(plan_scheduler/)
include_directories(meta/)

aux_source_directory(utils UTILS_SRC)
aux_source_directory(meta META_SRC)

add_executable(scheduler plan-scheduler/plan_scheduler.cpp plan-scheduler/plan_scheduler.h utils/json.hpp utils/base64.cpp ${META_SRC} ${PROTO_SRC} ${PROTO_HEADER})
add_executable(service plan-scheduler/service.cc ${META_SRC} ${UTILS_SRC} ${PROTO_SRC} ${PROTO_HEADER})


target_link_libraries(scheduler ${BRPC_LIB} ${MYSQL_LIB} ${DYNAMIC_LIB})
target_link_libraries(service ${BRPC_LIB} ${MYSQL_LIB} ${DYNAMIC_LIB})


     