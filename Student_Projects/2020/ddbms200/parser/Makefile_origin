
HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++` `mysql_config --cflags --libs`\
					 -pthread\
					 -ljson\
					 -lcurl\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/usr/local/lib `pkg-config --libs protobuf grpc++` `mysql_config --cflags --libs`\
           -pthread\
		   -ljson\
		   -lcurl\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
endif


all: main
main: Parse.o Parse_INIT.o Parse_LOAD.o Parse_SELECT.o Tools.o ../executor/cpp/executor_multi.o ../executor/cpp/mysql_connector.o ../transfer/transfer/transfer.pb.o ../transfer/transfer/transfer.grpc.pb.o ../transfer/transfer/client.o ../metadata/metadata.o
	$(CXX) $^ $(LDFLAGS) -o $@


clean:
	rm -f *.o main

