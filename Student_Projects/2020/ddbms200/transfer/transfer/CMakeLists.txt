cmake_minimum_required(VERSION 3.5.1)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_GRPCPP gRPC::grpc++)

#生成链接库
add_library(Client client.cc)
target_link_libraries(Client transfer.pb.o transfer.grpc.pb.o)
target_link_libraries(Client
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})

