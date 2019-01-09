一分布式数据库环境部署
    



    首先sudo apt-get update
    1.安装gcc
    sudo apt-get install gcc
    2.安装g++
    sudo apt-get install g++
    3.部署etcd
    1.下载etcd
    我下的是3.2.6版本
    wget https://github.com/coreos/etcd/releases/download/v3.2.6/etcd-v3.2.6-linux-amd64.tar.gz
    2.解压
    tar -zxvf  etcd-v3.2.6-linux-amd64.tar.gz -C /opt/
    cd /opt	
    mv etcd-v3.2.6-linux-amd64 etcd
    mkdir /etc/etcd 
    3.创建配置文件
    配置如下
    我配置3个节点
    10.41.46.184
    10.77.40.212
    10.79.10.210

    name: etcd-1
    data-dir: /opt/etcd/data
    listen-client-urls: http://10.41.46.184:2379,http://127.0.0.1:2379
    advertise-client-urls: http://10.41.46.184:2379,http://127.0.0.1:2379
    listen-peer-urls: http://10.41.46.184:2380
    initial-advertise-peer-urls: http://10.41.46.184:2380
    initial-cluster: etcd-1=http://10.41.46.184:2380,etcd-2=http://10.77.40.212:2380,etcd-3=http://10.79.10.210:2380
    initial-cluster-token: etcd-cluster-token
    initial-cluster-state: new

    name: etcd-2
    data-dir: /opt/etcd/data
    listen-client-urls: http://10.77.40.212:2379,http://127.0.0.1:2379
    advertise-client-urls: http://10.77.40.212:2379,http://127.0.0.1:2379
    listen-peer-urls: http://10.77.40.212:2380
    initial-advertise-peer-urls: http://10.77.40.212:2380
    initial-cluster: etcd-1=http://10.41.46.184:2380,etcd-2=http://10.77.40.212:2380,etcd-3=http://10.79.10.210:2380
    initial-cluster-token: etcd-cluster-token
    initial-cluster-state: new


    name: etcd-3
    data-dir: /opt/etcd/data
    listen-client-urls: http://10.79.10.210:2379,http://127.0.0.1:2379
    advertise-client-urls: http://10.79.10.210:2379,http://127.0.0.1:2379
    listen-peer-urls: http://10.79.10.210:2380
    initial-advertise-peer-urls: http://10.79.10.210:2380
    initial-cluster: etcd-1=http://10.41.46.184:2380,etcd-2=http://10.77.40.212:2380,etcd-3=http://10.79.10.210:2380
    initial-cluster-token: etcd-cluster-token
    initial-cluster-state: new

    这个配置文件是写在/etc/etcd/conf.yml里面作为启动文件
    vi /etc/etcd/conf.yml
    分别把上述的写到各自的站点
    编辑环境变量
    vi /etc/profile
    export ETCDCTL_API=3
    生效
    source /etc/profile
    那么现在单节点已经配置完成，现在把虚拟机拷贝复制

    cd  /opt/etcd
    查看版本
    ./etcdctl version
    启动
    ./etcd --config-file=/etc/etcd/conf.yml

    配置启动服务

    创建文件 /usr/lib/systemd/system/etcd.service


    添加如下内容
    [Unit]
    Description=Etcd Server
    After=network.target
    After=network-online.target
    Wants=network-online.target

    [Service]
    Type=notify
    WorkingDirectory=/opt/etcd/
    # User=etcd
    ExecStart=/opt/etcd/etcd --config-file=/etc/etcd/conf.yml
    Restart=on-failure
    LimitNOFILE=65536

    [Install]
    WantedBy=multi-user.target

    生成启动服务
    systemctl daemon-reload
    systemctl enable etcd
    systemctl start etcd
    systemctl restart etcd
    systemctl status etcd.service -l

    查看节点成员信息：
    ./etcdctl member list
    #[member]
    ETCD_NAME=default
    ETCD_DATA_DIR="/var/lib/etcd/default.etcd"
    ETCD_LISTEN_CLIENT_URLS="http://localhost:2379"
    ETCD_ADVERTISE_CLIENT_URLS="http://localhost:2379"
    [Unit]
    Description=Etcd Server
    After=network.target
    [Service]
    Type=simple
    WorkingDirectory=/opt/etcd/
    EnvironmentFile=-/etc/etcd/etcd.conf
    ExecStart=/opt/etcd/etcd
    [Install]
    WantedBy=multi-user.target
    4.部署curl
    wget http://curl.haxx.se/download/curl-7.40.0.tar.gz
    tar zxvf curl-7.40.0.tar.gz 
    cd curl-7.40.0/
    ./configure 
    make && make install
    接着，将curl 7.40.0/include/curl文件复制到 /usr/include下面
    gcc ftpsget.c -o ddd.out -lcurl编译的时候记得+ -lcurl

    出现curl: error while loading shared libraries: libcurl.so.4: cannot open shared object file: No such file or directory错误
    去寻找/etc/ld.so.conf添加
    /usr/local/lib
    再执行（root用户）
    /sbin/ldconfig -v
    5.部署json
    安装scons工具
    wget https://jaist.dl.sourceforge.net/project/scons/scons/2.5.0/scons-2.5.0.tar.gz
    tar -zxvf scons-2.5.0.tar.gz
    python setup.py install
    下载jsonapp安装包
    wget https://jaist.dl.sourceforge.net/project/jsoncpp/jsoncpp/0.5.0/jsoncpp-src-0.5.0.tar.gz
    tar -zvxf jsoncpp-src-0.5.0.tar.gz
    scons platform=linux-gcc
    将/jsoncpp-src-0.5.0/include/目录下的json文件夹拷贝到 /usr/include/
    到jsonapp目录下
    cd /Downloads/jsoncpp-src-0.5.0/libs/linux-gcc-4.8.5
    将libjson_linux-gcc-4.8.5_libmt.a拷贝到/usr/local/lib下
    cp -r libjson_linux-gcc-4.8.5_libmt.a /usr/local/lib
    在/usr/local/lib目录下将其重命名为：libjson.a
    mv libjson_linux-gcc-4.8.5_libmt.a libjson.a
    6.安装boost
        sudo apt-get install libboost-dev
    7.在18.04中pie变成了默认了，需要在cmakelist中添加-no-pie
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -std=c++11 -ljson -lcurl -no-pie")
    修改你cmake里面各种路径
    8安装mysql
    　1. sudo apt-get install mysql-server
    
    　　2. sudo apt-get install mysql-client
    
    　　3.  sudo apt-get install libmysqlclient-dev
    更改mysql 的root密码
    首先
    sudo vim /etc/mysql/mysql.conf.d/mysqld.cnf,
    在mysqld下添加
    skip-grant-tables
    保存:wq，退出。输入：service mysql restart，重新启动mysql。
    输入mysql -u root -p
    输入use mysql
    update user set authentication_string=password("456789"),plugin='mysql_native_password' where user='root'
    接着sudo vim /etc/mysql/mysql.conf.d/mysqld.cnf
    注释skip-grant-tables
    在输入service mysql restart重启mysql
    在mysql中创建4个db
    create database db1;
    create database db2;
    create database db3;
    create database db4;
二、运行环境
我们的分布式数据库采用 C++语言开发，运行在 Ubuntu 18.04 上，底层的 数据库使用 MySQL5.7，并且使用了 Etcd 管理元数据信息。采用了 P2P 的架构， 每个 Server 都可以部署在任意的机器上，并且每一个节点都可以作为客户端程 序进行访问。我们的实验环境使用了三台机器，在 PC1，PC2 上分别部署了一个 站点，在 PC3 上使用不同的端口部署了多个站点。
三、代码结构说明

    metadata.cpp 是负责元数据管理，主要提供存储etcd功能；
    query_tree_with_fragment.cpp 是负责查询计划
    sql_exec.cpp 是负责sql执行
    rpc_sql.cpp 是负责分发sql到各个站点之中
    rpc_server.cpp 是启动rpc服务
    parser.cpp 解析sql语句
    local_sql.cpp 本地sql语句封装
    main.cpp 主函数
