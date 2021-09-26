<!-- 先在executor目录下编译executor生成.o文件 -->
g++ -c mysql_connector.cpp `mysql_config --cflags --libs` ../../metadata/metadata.cc -lcurl -ljson

<!-- 在该目录下 生成server , client-->
make

<!-- 开启服务 -->
./server

<!-- 开启服务以后，在另外一个终端执行请求 -->
./client

<!-- 安装gRPC -->
https://www.jianshu.com/p/73ec0014ac0f