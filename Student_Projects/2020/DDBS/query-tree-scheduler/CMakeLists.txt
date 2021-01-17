cmake_minimum_required(VERSION 2.9.10)
project(query-tree-scheduler C CXX)

include(FindThreads)
include(FindProtobuf)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER proto/ddb.proto)



include_directories(${CMAKE_CURRENT_BINARY_DIR})

# inner include
include_directories(include)

# brpc
set(CMAKE_PREFIX_PATH ../../third-party/brpc/lib/)
find_library(BRPC_LIB NAMES libbrpc.a brpc)
include_directories(../../third-party/brpc/include)

# mysql-connector
set(CMAKE_PREFIX_PATH ../../third-party/mysql/lib)
find_library(MYSQL_LIB NAMES mysqlcppconn)
include_directories(../../third-party/mysql/include)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -DNDEBUG -O2 -D__const__= -pipe -W -Wall -Wno-unused-parameter -fPIC -fno-omit-frame-pointer")

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

# leveldb
find_path(LEVELDB_INCLUDE_PATH NAMES leveldb/db.h)
find_library(LEVELDB_LIB NAMES leveldb)
if ((NOT LEVELDB_INCLUDE_PATH) OR (NOT LEVELDB_LIB))
    message(FATAL_ERROR "Fail to find leveldb")
endif()
include_directories(${LEVELDB_INCLUDE_PATH})

include_directories(${OPENSSL_INCLUDE_DIR})
find_package(OpenSSL)



add_library(network-utils SHARED src/network.cpp src/base64.cpp ${PROTO_SRC} ${PROTO_HEADER})

set(DYNAMIC_LIB
    ${CMAKE_THREAD_LIBS_INIT}
    ${GFLAGS_LIBRARY}
    ${PROTOBUF_LIBRARIES}
    ${LEVELDB_LIB}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
    dl
    )


#target_include_directories(network-utils PUBLIC include)
target_link_libraries(network-utils ${BRPC_LIB} ${DYNAMIC_LIB})


add_executable(query-loader src/load_query.cpp src/base64.cpp)
add_executable(query-scheduler src/schedule_query.cpp src/base64.cpp ${PROTO_SRC} ${PROTO_HEADER} src/MysqlConnectionPool.hpp)
add_executable(query-executer src/execute_query.cpp ${PROTO_SRC} ${PROTO_HEADER})


target_link_libraries(query-loader ${BRPC_LIB} ${DYNAMIC_LIB})
target_link_libraries(query-scheduler ${BRPC_LIB} ${MYSQL_LIB} ${DYNAMIC_LIB})
target_link_libraries(query-executer ${BRPC_LIB} ${DYNAMIC_LIB})

     