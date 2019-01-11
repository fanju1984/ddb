# gDB
A distributed database for course design, written in golang.

#代码结构
项目的文件夹包括：

* main文件夹：程序入口。只有接收命令行参数，调用各服务的启动函数和监听程序结束的信号的代码在这里。
* server文件夹：服务器相关的代码。包括了整个服务器的生命周期的管理，命令行参数读入后的处理和一些工具函数。
* storage文件夹：存取数据相关的代码。
* pb文件夹：proto文件和生成的go文件存放的文件夹。
* dataManager：执行除了Select以外语句的代码。
* exprParse：解析除了Select以外语句的代码。
* gddGet：元数据存取的第二层包装。
* SQL：Select的解析和执行。

# 部署方式
1. 安装pg和golang环境。
2. go install gDB/main
3. 配置toml文件，配置项如下： name, data-dir, client-urls, peer-urls, initial-cluster, initial-cluster-state, pg-username, pg-password, pg-bin-path, pg-data-path, pg-port, log-level, grpc-urls, grpc-peers。
4. 通过--config ../configfile.toml参数启动

配置文件参考：

```
name = "st0"

client-urls = "http://127.0.0.1:2379"
peer-urls = "http://127.0.0.1:2380"

initial-cluster = "st0=http://127.0.0.1:2380,st1=http://127.0.0.1:2382,st2=http://127.0.0.1:2384,st3=http://127.0.0.1:2386"
initial-cluster-state = "new"

grpc-urls = "localhost:2480"
grpc-peers = "st0=localhost:2480,st1=localhost:2481,st2=localhost:2482,st3=localhost:2483"

pg-username = "cmk"
pg-password = ""
pg-bin-path = "C:\\Program Files\\PostgreSQL\\11\\bin"
pg-data-path = "F:\\running_test\\test_"
data-dir="F:\\running_test\\etcd0"
pg-port = 5437

log-level = "debug"
```