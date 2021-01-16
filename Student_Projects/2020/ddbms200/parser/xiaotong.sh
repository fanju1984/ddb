g++ -c Parse.cpp
g++ -c Parse_LOAD.cpp
g++ -c Parse_INIT.cpp
g++ -c Parse_SELECT.cpp
g++ -c Parse_INSERT.cpp
g++ -c Tools.cpp
g++ Parse.o Parse_INIT.o Parse_LOAD.o Parse_SELECT.o Tools.o ../executor/cpp/executor_multi.o ../executor/cpp/mysql_connector.o ../transfer/transfer/transfer.pb.o ../transfer/transfer/transfer.grpc.pb.o ../transfer/transfer/client.o ../metadata/metadata.o -o main `pkg-config --cflags protobuf grpc` `pkg-config --libs protobuf grpc++` `mysql_config --cflags --libs` -ljson -lcurl
# g++ Parse_INSERT.o Tools.o ../metadata/metadata.o -ljson -lcurl -o insert