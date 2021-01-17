cmake_minimum_required(VERSION 2.9.10)
set(CMAKE_CXX_STANDARD 11)
project(DataLoader C CXX)

# 查找当前目录下的所有源文件, 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 指定生成 DataLoader 链接库
add_library(DataLoader ${DIR_LIB_SRCS})
