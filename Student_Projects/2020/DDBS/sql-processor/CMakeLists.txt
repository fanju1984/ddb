cmake_minimum_required(VERSION 2.9.10)
set(CMAKE_CXX_STANDARD 11)
project(SQLProcessor C CXX)

# sql-parser
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/sql-parser/lib)
find_library(SQL_PARSER_LIB NAMES libsqlparser.so)
# message(STATUS ${SQL_PARSER_LIB})

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 SQLProcessor 链接库
add_library(SQLProcessor ${DIR_LIB_SRCS})

# 将目标文件 SQLProcessor 与第三方库文件 SQL-Parser 进行链接
target_link_libraries(SQLProcessor ${SQL_PARSER_LIB})

# 子项目不用设置头文件的目录，直接在程序中用相对路径引用头文件即可
# set(SQL_PARSER_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../../third-party/sql-parse/include)
# message(STATUS ${SQL_PARSER_INCLUDE})
# include_directories(${SQL_PARSER_INCLUDE})
# target_include_directories(SQLProcessor PRIVATE ${SQL_PARSER_INCLUDE})
