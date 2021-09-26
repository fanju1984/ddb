

吴坤尧 2020100359（组长）

王大林 2020104182

水治禹2019101380

赵泓尧2019101386

1. iddb简介

本次课程作业实现了iddb，一个使用go语言编写的分布式关系数据库。

用户可以在任意站点上启动客户端，输入SQL语句，iddb能将其转化为一棵考虑了数据分片信息的查询树，在各相关站点上完成查询，最后再汇总为最终结果。

iddb能够实现以下功能：

    1. 处理INSERT语句，依据分片信息将数据存储到正确的站点上。
    2. 处理DELETE语句，能够正确删除数据。
    3. 处理SELETE语句，并考虑了简单的传输优化、算子下推、连接优化。benchmark中的11个query的平均查询时间为4.47s，最慢不超过8s。
    4. 能够在无关站点离线时正常执行查询。



2. 方案设计
# 2.1 系统架构图

![图片](https://uploader.shimo.im/f/DmF9hnMpZPJ5zLfo.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

整体的方案设计沿用了中期展示的方案，在实现时有微小的调整。

整个系统分为个主要模块：iparser、iqueryanalyzer、iqueryoptimizer、irpc、imeta

和iexecutor。正常运行时，除iddb外需要部署并启动mysql和etcd。

iddb处理用户输入的sql语句的流程如下：

    1. 用户可以在任意站点上启动client，输入sql语句
    2. iparser接收用户输入的sql语句，将其转换为AST(Abstract Syntax Tree),注意，这里的AST不是树结构，仅仅是一个分解出表名、列名、选择条件等信息的结构体。
    3. iqueryanalyzer解析AST，结合分片信息，构建出一棵包含实际站点的二叉查询树。
    4. iqueryoptimizer内包含一组优化函数。这组函数具有相同的特点，即输入是查询树，输出也是查询树。
    5. 通过imeta模块封装好的etcd模块，将查询树写入etcd中。
    6. 通过irpc模块，向所有相关站点发送执行任务的请求。
    7. 相关站点的iexecutor被调用，从imeta中获取查询树，找到自己能执行的第一个结点，执行相应的操作,与本地mysql进行交互。当自己所有任务都完成时，执行结束。
    8. 当所有站点的iexecutor都完成任务时，最终结果返回到用户所在站点。

在具体实现时，由于irpc和iexecutor会出现循环引用，故将irpc和iexecutor各拆解成两部分；另外还加入了处理插入语句的ihandleinsert模块、处理删除语句的ihandledelete模块、提供公用数据结构和接口的iplan和iutilities。

各模块均有相应的单元测试，位于iddb/src/examples/，可以在里面查看相应函数的用法和结果。

# 2.2 模块设计

![图片](https://uploader.shimo.im/f/W1yXuLSyWmC7Kcv5.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

### iddb server: （吴）

主要功能：根据配置文件，启动分布式数据库服务

调用关系：调用irpctran server和irpccall server

### iddb client: （吴）

主要功能：提供用户交互界面，向iddb server发送请求

调用关系：调用iparser、iqueryanalyzer、iqueryoptimizer、imeta和irpccall client

### Irpccall: （吴）

主要功能：client通知各server开始工作

调用关系：irpccall 的client被iddb的client调用，irpccall 的server调用iexec

### Irpctran: （吴）

主要功能：各site之间传输数据

调用关系：被iexec调用

### iparser:（王）

主要功能：将sql语句进行分析，生成抽象语法树AST，再根据AST生成逻辑查询计划

调用关系：iparser被client调用

### iqueryanalyzer：(王）

主要功能：将iparser生成的逻辑查询计划与imeta中的分片信息相结合，生成物理查询计划

调用关系：iqueryanalyzer被client调用

### iqueryoptimizer: (王、吴)

主要功能：将iqueryanalyzer生成的物理查询计划进行优化处理，生成优化过后的查询计划

调用关系：iqueryoptimizer被client调用

### iexec:(赵)

主要功能：server接收到客户端生成的执行计划树，在mysql中进行执行。

调用关系：irpccall的server调用iexec，iexec调用irpccall进行数据传输

### itrans:(赵)

主要功能：iexec中单独用来传输数据的模块，用来将一台机器上的数据传输到另外一台。

调用关系：irpccall被iexec调用传输数据，irpccall再调用itrans将传输的数据存入mysql

### iplan:(水)

主要功能：语法树、节点和分片信息的数据结构。

调用关系：被 imeta、iqueryanalyzer、iqueryoptimizer、iexec 调用，作为数据结构。

### imeta:(水)

主要功能：与 etcd 交互的接口，主要有存取语法树和分片信息。

调用关系：调用 iplan 中的数据结构，在查询阶段会调用此接口。

# 3. 环境部署
# 3.1etcd安装配置

```plain
environment
(1)packages:
$cd ~/iddb/src/github.com/coreos
$git clone https://github.com/coreos/go-semver.git
$git clone https://github.com/coreos/go-systemd.git
$cd ~/iddb/src/github.com/gogo
$git clone https://github.com/gogo/protobuf.git
$cd ~/iddb/src/go.uber.org
$git clone https://github.com/uber-go/zap.git
$git clone https://github.com/uber-go/multierr.git
$git clone https://github.com/uber-go/atomic.git
(2)etcd:
$cd ~/iddb/src/github.com/coreos
$git clone https://github.com/etcd-io/etcd.git
for version 3.3.25
$cd etcd
$git checkout 7d12776
$./build
(3)goreman (for cluster)
cd ~/iddb/src/github.com/coreos
$git clone https://github.com/mattn/goreman.git
$make
if fialed =>
$go env -w GOPROXY=https://goproxy.cn,direct
$go env -w GOPRIVATE=.gitlab.com,.gite.com
$go env -w GOSUMDB="sum.goolang.google.cn"
(4)configfile: etcd/Procfile_C
# Procfile_C
#161
etcd1: bin/etcd --name etcd161 --listen-client-urls http://10.77.70.161:2379,http://127.0.0.1:2379 --advertise-client-urls http://10.77.70.161:2379 --listen-peer-urls http://10.77.70.161:2380 --initial-advertise-peer-urls http://10.77.70.161:2380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'etcd161=http://10.77.70.161:2380,etcd162=http://10.77.70.162:2380,etcd171=http://10.77.70.171:2380' --initial-cluster-state new --enable-pprof
#162
etcd2: bin/etcd --name etcd162 --listen-client-urls http://10.77.70.162:2379,http://127.0.0.1:2379 --advertise-client-urls http://10.77.70.162:2379 --listen-peer-urls http://10.77.70.162:2380 --initial-advertise-peer-urls http://10.77.70.162:2380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'etcd161=http://10.77.70.161:2380,etcd162=http://10.77.70.162:2380,etcd171=http://10.77.70.171:2380' --initial-cluster-state new --enable-pprof
#171
etcd3: bin/etcd --name etcd171 --listen-client-urls http://10.77.70.171:2379,http://127.0.0.1:2379 --advertise-client-urls http://127.0.0.1:2379 --listen-peer-urls http://10.77.70.171:2380 --initial-advertise-peer-urls http://10.77.70.171:2380 --initial-cluster-token etcd-cluster-1 --initial-cluster 'etcd161=http://10.77.70.161:2380,etcd162=http://10.77.70.162:2380,etcd171=http://10.77.70.171:2380' --initial-cluster-state new --enable-pprof
(5)run etcd cluster
```
主要是安装 etcd-id、goreman，配置文件 Procfile_C；环境部分依需要而定。
下面是 etcd 的启动方式：

```plain
1）在使用前，需要先启动集群：
a.打开目录： 
  $cd ~/iddb/src/github.com/coreos/etcd
b.启动集群 分别在三台机器下运行启动，不建议使用后台运行。
  $../../mattn/goreman/goreman -f Procfile_C start 
```
# 3.2mysql安装配置

在mysql官网下载mysql-5.7.32，并将其放到三台服务器的/home/centos目录下

首先在/home/centos下创建好mysql_data目录，并且将mysql解压

```shell
tar -zxvf mysql-5.7.32-linux-glibc2.12-x86_64.tar.gz
mv mysql-5.7.32-linux-glibc2.12-x86_64 mysql
mkdir mysql_data
```
接下来安装mysql
```shell
cd mysql/bin
./mysqld --user=centos --basedir=/home/centos/mysql/ --datadir=/home/centos/mysql_data --initialize
```
修改配置文件
```shell
cd /home/centosmysql/support-files
cp my-default.cnf /etc/my.cnf
cp mysql.server /etc/init.d/mysql
vim /etc/my.cnf
```
修改以下内容
![图片](https://uploader.shimo.im/f/2fJ7Qe2jim1jTUzW.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

```shell
准备启动mysql
ln -s /home/centos/mysql/bin/mysql /usr/bin
service mysql start
mysql -uroot -p
进入 mysql 修改密码
mysql> set password=password('root');
mysql> flush privileges;
```
# 3.3grpc+protobuf

下载 protobuf：

git clone[https://github.com/protocolbuffers/protobuf.git](https://github.com/protocolbuffers/protobuf.git)

sudo apt-get install autoconf  automake  libtool curl make  g++  unzip libffi-dev -y

cd protobuf/

./autogen.sh

./configure

make

sudo make install

sudo ldconfig

protoc -h

下载和安装grpc

git clone[https://github](https://github).com/grpc/grpc-go.git $GOPATH/src/google.golang.org/grpc

git clone[https://github](https://github).com/golang/net.git $GOPATH/src/golang.org/x/net

git clone[https://github](https://github).com/golang/text.git $GOPATH/src/golang.org/x/text

go get -u github.com/golang/protobuf/{proto,protoc-gen-go}

git clone[https://github](https://github).com/google/go-genproto.git $GOPATH/src/google.golang.org/genproto

cd $GOPATH/src/

go install google.golang.org/grpc

使用:

先编写.proto文件

protoc --go_out=. --go-grpc_out=. irpc.proto

命令正常执行后，irpc.proto文件同级目录下将会出现两个.pb.go文件，rpc的数据结构、接口定义完毕。

![图片](https://uploader.shimo.im/f/v34vTgr4xwVaY7SU.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)





# 3.4golang环境配置

下载golang源码

cd /home/centos

mkdir GoSrc

cd GoSrc

wget[https://studygolang.com/dl/golang/go1.15.4.linux-amd64.tar.gz](https://studygolang.com/dl/golang/go1.15.4.linux-amd64.tar.gz)

tar zxf[go1.15.4.linux-amd64.tar.gz](https://studygolang.com/dl/golang/go1.15.4.linux-amd64.tar.gz)

sudo mv go /usr/local/

配置并刷新环境变量

vim /etc/profile

export GOROOT=/usr/local/go

export GOBIN=$GOROOT/bin

export PATH=$PATH:$GOBIN

export GOPATH=/home/centos/iddb

source /etc/profile

输入go env，可以检查以上环境变量是否正确设置

# 3.5配置文件

配置文件模版位于iddb/cnf/，为了防止每次git pull时不同站点的配置文件被统一化，需要将配置文件移动到iddb工程外的目录。

cp cnf/iddb.conf /home/centos/config

![图片](https://uploader.shimo.im/f/nLsRQWW6kxtbV9lm.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

配置文件内包含了以下信息：

1. 各站点的ip、各服务的端口号
2. 本站点的id
3. 本地mysql的账号、密码等信息

![图片](https://uploader.shimo.im/f/Iwy1JUALyWKYEOUf.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

![图片](https://uploader.shimo.im/f/tqR72J1WW9sGpaoB.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

![图片](https://uploader.shimo.im/f/SJ6Ak1jg8hlJon1w.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

配置文件将在每次server和client启动时被读取，读取配置文件操作被iutilities内的LoadAllConfig函数封装。

![图片](https://uploader.shimo.im/f/zptVgbCW9Y4iLaO3.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)




4. 系统实现
# 4.1 iparser+iqueryanalyzer+iqueryoptimizer(王)

## 4.1.1 iparser

iparser代码文件位于iddb/iparser

iparser的核心功能有两个：

1. 接收字符串形式的sql语句，先处理成抽象语法树AST
2. 然后根据AST生成初始版本的逻辑查询计划

第一个核心功能的实现我们决定采用github上已经实现的功能相对完善的sql parser。

通过import github.com/xwb1989/sqlparser，我们引入了sqlparser包，随后即可通过调用sqlparser.Parse()函数来实现从sql语句到AST的转换。

第二个核心功能，需要对上一步解析出的AST抽象语法树进行信息提取，根据我们事先约定好的查询计划树的数据结构（查询计划树的数据结构位于iddb/iplan.go）进行信息填充，从而构建出逻辑查询计划树。

这里的第一个难点在于对AST信息的提取。由于我们采用了GitHub上现成的sqlparser，因此我需要阅读该项目的源代码才能知道我该如何调用AST中的信息。AST的数据结构形式如下：

```go
type Select struct {
    Cache       string
    Comments    Comments
    Distinct    string
    Hints       string
    SelectExprs SelectExprs
    From        TableExprs
    Where       *Where
    GroupBy     GroupBy
    Having      *Where
    OrderBy     OrderBy
    Limit       *Limit
    Lock        string
}
```
但是由于该项目的实现中大量的使用了interface功能，使得我直接调用该数据结构中的数据无法实现，一时间陷入了巨大的瓶颈。随后在查阅资料的过程中，一篇博客中引用了与我相同的parser包，我查看他的调用方式，发现了sqlparser.String()函数，可以直接将对应的部分转换为字符串的形式，虽然这样的信息提取方式虽然与AST的理念背道而驰，将原本分开的信息合并为字符串形式，但是至少可以把select语句的不同部分分开。我们后续通过字符串的处理也可以达到我们想要的效果。
第二个难点在于如何确定查询树的生成逻辑。当有多个表参与查询时，需要先读出表的数量，如果是4个表，那么为了后面的查询速度的优化，直接建立平衡树，否则的话，就每次插入一个表，形成三角形向上堆叠。

```go
if len(sel.From) == 4 {
        println("handle 4 tables!!!!")
        buildBalanceTree()
} else {
    for _, table := range sel.From {
        tableName := sqlparser.String(table)
        AddTableNode(CreateTableNode(tableName))
    }
}
```
## 4.1.2 iqueryanalyzer

iqueryanalyzer位于iddb/iqueryanalyzer

主要功能：结合分片信息，调整iparser得到的逻辑查询计划，生成物理查询计划。实际上就是一个表的拆分。根据获取的分片信息，将每个大表拆成由站点组成的小表。

在拆表的同时，还需要填充一组新的信息到树的节点中，也就是传输相关的信息。locate，transferfalg，dest。这里的关键是locate的获取，transferfalg和dest都是根据locate来填充的。初始时，只有每个表有locate，随后利用递归，传递locate信息

```go
func getLocate(i int64) (locate int64) {
    if physicalPlanTree.Nodes[i].Locate != -1 {
        locate = physicalPlanTree.Nodes[i].Locate
    } else if physicalPlanTree.Nodes[i].Left != -1 && physicalPlanTree.Nodes[i].Right != -1 {
        //如果存在 client 站点，则优先 client，否则取两个孩子中 Locate 小的那个
        var minchildinfo string
        locate, minchildinfo = min(getLocate(physicalPlanTree.Nodes[i].Left), getLocate(physicalPlanTree.Nodes[i].Right))
        switch minchildinfo {
        case "Left":
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].TransferFlag = false
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].TransferFlag = true
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].Dest = locate
        case "Right":
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Right].TransferFlag = false
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].TransferFlag = true
            physicalPlanTree.Nodes[physicalPlanTree.Nodes[i].Left].Dest = locate
        case "Equal":
        }
    } else if physicalPlanTree.Nodes[i].Left == -1 && physicalPlanTree.Nodes[i].Right != -1 {
        locate = getLocate(physicalPlanTree.Nodes[i].Right)
    } else if physicalPlanTree.Nodes[i].Left != -1 && physicalPlanTree.Nodes[i].Right == -1 {
        locate = getLocate(physicalPlanTree.Nodes[i].Left)
    } else {
        fmt.Println("error when getlocate, there is a node without child but don't have locate")
        fmt.Printf("nodeid is %d\n", i)
        os.Exit(1)
    }
    return locate
}
```
## 4.1.3 iqueryoptimizer

iqueryoptimizer位于iddb/iqueryoptimizer

主要功能：对物理查询计划进行优化，进行选择下推，剪枝，传输优化等优化操作

由于时间关系，我们只实现了选择下推。在进行选择下推之前，有一些前置工作需要进行。第一，我们需要得到每个节点的临时表所包含的列。第二，我们需要把选择条件根据and拆分成若干个小条件。完成之后，我们从选择节点开始向下试探，如果该节点满足：执行选择需要的列都被包含在其临时表中，则表示选择条件可以加在该节点之上。然后利用递归，找到最下面的可以插入的位置，并把选择条件插入即可。

```go
func tryPushDown(subWhere string, beginNode int64) {
    pos := find2ChildNode(beginNode)
    if pos == -1 { //若为-1 则说明没有两个孩子的节点，只能加在 curPos 上
        addWhereNodeOnTop(iparser.CreateSelectionNode(iparser.GetTmpTableName(), subWhere), beginNode)
    } else {
        flag1 := checkCols(subWhere, pt.Nodes[pt.Nodes[pos].Left].Rel_cols)
        flag2 := checkCols(subWhere, pt.Nodes[pt.Nodes[pos].Right].Rel_cols)
        if flag1 == false && flag2 == false {
            addWhereNodeOnTop(iparser.CreateSelectionNode(iparser.GetTmpTableName(), subWhere), pos)
        }
        if flag1 == true {
            tryPushDown(subWhere, pt.Nodes[pos].Left)
        }
        if flag2 == true {
            tryPushDown(subWhere, pt.Nodes[pos].Right)
        }
    }
}
```
# 4.2 imeta+iHandleDelete+iHandleInsert（水）

## **4.2.1 imeta**

前期工作主要集中在 imeta 上 ，主要是对 etcd 同步数据的的配置以及接口的编写。

1. 配置安装 etcd 并启动集群，测试集群的健康状态（./etcdctl member list），检测数据是否同步成功；
2. 在本处所设计存入 etcd 的内容包括语法树和分片信息，数据结构在 ipan/plan.go 中，根据数据结构（结构体数组），对用 etcd 的 key-value 模式，在 etcd 中使用扁平化存储模式，如下图。在 etcd 中/txn id 则的 value 存储 node 节点最大值，以/txn id/node id 为每棵语法树节点的 key，value 存储一系列详细信息，用分隔符分割字符串。

![图片](https://uploader.shimo.im/f/EJoXZ0GDWALUNhVh.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

![图片](https://uploader.shimo.im/f/LKsGxPqe5APRac09.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

3. 使用 go 和 etcd 交互的接口，github.com/coreos/etcd/clientv3 中的 clientv3.NewKV(*clientv3.Client).Get 和clientv3.NewKV(*clientv3.Client).Put 等接口，完成使用 go 语言调用 etcd；
4. 字符串、数据结构等处理，完成 etcd 数据存储格式与其他部分调用所需数据格式的转换，提高易于操作的接口。

主要完成了以下接口，etcd 内部情况对外透明：

```plain
interface:
一、查询树
imeta/imeta.go: Build_Txn(txnid int64) error
imeta/imeta.go: Set_Node(txnid int64, Node_in iplan.PlanTreeNode) error
imeta/imeta.go: Set_Tree(txnid int64, Tree_in iplan.PlanTree) error
imeta/imeta.go: Get_Node(txnid int64, nodeid int64) (iplan.PlanTreeNode, error)
imeta/imeta.go: Get_Tree(txnid int64) (iplan.PlanTree, error)
二、分片信息
imeta/imeta.go: Set_FragTree(tablename string, Treey_in iplan.FragTree) error
imeta/imeta.go: Get_FragTree(tablename string) (iplan.FragTree, error)
```
代码截取片段进行解释：
```plain
imeta/imeta.go
var Cli *clientv3.Client
//声明全局变量，供全局使用
func Connect_etcd(){
  config := clientv3.Config{ //配置 etcd client 的条件
    Endpoints: []string{"localhost:2379"},
    DialTimeout: 15 * time.Second,
  }
  var err error
  if Cli,err = clientv3.New(config);err!=nil {
    //与 etcd 建立一个新的连接
    fmt.Println(err.Error())
    return
}
  fmt.Println("etcd connect succ")
}
...
//以 Set_Node 举例
func Set_Node(txnid int64, Node_in iplan.PlanTreeNode)(error){
    kv := clientv3.NewKV(Cli)
    //使用 kv 存储的接口，首先将 client 连接转为 kv 模式
        if(int(Node_in.Nodeid) < 0){
        Node_in.Nodeid  = int64(0)
    }
    ikey :=  "/"+strconv.FormatInt(txnid,10)+"/"+strconv.FormatInt(Node_in.Nodeid,10)
    //下面进行字符串处理，使用"##"作为每个节点不同属性之间的分隔符，然后存入 etcd
```
字符串整串存储而不是分属性存储可以提高与 etcd 交互的效率，从而减小在这一部分的时间开销。因为我们的策略是整个查看使用并更新全部返回的，不关注单个属性 而是一次考虑全部熟悉。
```plain
    var ivalue string
    ivalue =  strconv.FormatInt(Node_in.Nodeid,10)+"##"+strconv.FormatInt(Node_in.Left,10)+"##"+strconv.FormatInt(Node_in.Right,10)+"##"
    ivalue = ivalue+strconv.FormatInt(Node_in.Parent,10)+"##"+strconv.FormatInt(Node_in.Status,10)+"##"+Node_in.TmpTable+"##"+strconv.FormatInt(Node_in.Locate,10)+"##"
    ivalue = ivalue+strconv.FormatBool(Node_in.TransferFlag)+"##"+strconv.FormatInt(Node_in.Dest,10)+"##"+strconv.FormatInt(Node_in.NodeType,10)+"##"
    ivalue = ivalue+Node_in.Where+"##"+Node_in.Rel_cols+"##"+Node_in.Cols+"##"+strconv.FormatInt(Node_in.Joint_type,10)+"##"+Node_in.Joint_cols
    
//调用库函数接口，将节点及其值放入 etcd 中
    if putResp, err := kv.Put(context.TODO(),ikey,ivalue,clientv3.WithPrevKV());err !=nil {
        fmt.Println(err)
        return  err
    }else{
        if putResp.PrevKv !=nil {
            fmt.Println("Update:")
            fmt.Println(string(putResp.PrevKv.Value))
        }
        //iplan.PlanTree NodeNum at /txnid
        //此处取出/txnid 处的值，记录的是总的 node 的最大值，如果当前插入的 node 更大，则更新。
        node0 :=  "/"+strconv.FormatInt(txnid,10)
```
由于在形成查询树的过程中，树不是一棵满树，因此，节点并不一定都有值。nodenum 不再记录 node 的数目，而是记录 node id 的最大值。
另外，不同的/txn 前缀保证了即使进行多个查询，即多个事务，其 etcd 查询树也是完全独立互不干扰的，不会造成影响。因此，理论上可以并发进行多个查询。

```plain
  //放置 node num 或者说 MAX(node id)
        if getResp, err := kv.Get(context.TODO(),node0);err !=nil {
            fmt.Println(err)
            return err
        }else{
            s0 := string(getResp.Kvs[0].Value)
            sep0 := "##"
            value0 := strings.Split(s0,sep0)
            if(len(value0)<2) {
                fmt.Println("Plantree has no root or node num")
            }
            var oldnum int64
            oldnum, err = strconv.ParseInt(value0[0], 10, 64)
            if(Node_in.Nodeid>oldnum){
                newnum := strconv.FormatInt(Node_in.Nodeid, 10)
                value00 := newnum+"##"+value0[1]                kv.Put(context.TODO(),node0,value00,clientv3.WithPrevKV())
            }   
        }
    }
    return nil
}
```
其余操作调用的函数可能有所不同，但大致与此类似，就不再赘述。
## 4.2.2 iHandleDelete +iHandleInsert

这一部分需要调用到查询树中的部分函数接口，同时借助分片信息来完成语句的分发。

这一部分语句有特殊性，不同于 select 语句的复杂性。比如，delete 部分，在水平分片的情况下，(a)可以进行分片判断后向该分片发送删除信息，(b)也可以直接向每个 site 都发送删除信息。两种选择会有不同的开销，在这里，考虑到发送四个站点并各自删除是并行执行，并不会比删除一个站点开销增大很多，而判断在哪一个站点可能会耗费更大开销，我们选择除了垂直分片的 customer 表，其余表使用向全部站点发送 delete 语句的方式。

如：

```plain
func HandleDelete(sql string) (int64, [8]string, [8]int64) {
	var TotalNum int64
	var outsql [8]string
	var siten [8]int64
	stmt, err := sqlparser.Parse(sql)
	if err != nil {
		// Do something with the err
		println("ERROR with parser!\n")
	}
	sel := stmt.(*sqlparser.Delete)
	tablename := sqlparser.String(sel.TableExprs)
	tablename = strings.ToLower(tablename)
	fmt.Println(tablename)
	switch tablename {
	case "orders":
		TotalNum = 4
		i := 0
		for i < int(TotalNum) {
			siten[i] = int64(i)
			strwhere := sqlparser.String(sel.Where)
			strwhere = strings.Replace(strwhere, "customer_id", "ocid", -1)
			strwhere = strings.Replace(strwhere, "book_id", "obid", -1)
			outsql[i] = "delete from orders_" + strconv.Itoa(i) + strwhere
			i = i + 1
		}
	case "book":
...
```
而对于垂直分片的表，由于删除语句的条件可能需要同时在两边都满足，因此则需要先走查询的方式，查询后得出 id 再进行删除。分为两个阶段，即：查询并返回值，删除。如下：
```go
...	
    case "customer":
		//step1 find cid
		strwhere := sqlparser.String(sel.Where)
		strwhere = strings.Replace(strwhere, "id", "cid", -1)
		strwhere = strings.Replace(strwhere, "name", "cname", -1)
		sqlstmt := "select cid from customer" + strwhere
		//txnID needs to be unique!
		txnID++
		plantree := iparser.Parse(sqlstmt, txnID)
		plantree = iqueryanalyzer.Analyze(plantree)
		plantree = iqueryoptimizer.Optimize(plantree)
		ipaddr0 := iutilities.Peers[0].IP + ":" + iutilities.Peers[0].Call
		ipaddr1 := iutilities.Peers[1].IP + ":" + iutilities.Peers[1].Call
		plantree.Print()
		imeta.Connect_etcd()
		println("start imeta")
		err = imeta.Build_Txn(txnID)
		if err != nil {
			iutilities.CheckErr(err)
		}
		println("imeta build txn ok")
		err = imeta.Set_Tree(txnID, plantree)
		if err != nil {
			iutilities.CheckErr(err)
		}
		println("imeta set tree ok")
		println("end imeta")
		iutilities.Waitgroup.Add(1)
		go irpccall.RunCallClient(ipaddr0, txnID)
		iutilities.Waitgroup.Add(1)
		go irpccall.RunCallClient(ipaddr1, txnID)
		iutilities.Waitgroup.Wait()
		plantree, err = imeta.Get_Tree(txnID)
		if err != nil {
			iutilities.CheckErr(err)
		}
		res := iexec.GetResult(plantree, txnID)
		println(res)
```
此处已经完成了查询并返回了值的集合 res，在下面以此条件进行删除即可。不过，删除语句需要根据分片信息进行一定程度的修改。
```plain
		//res :=[] int {11,22,33 }
		//step2 delete by cid
		//TotalNum = 4
		if len(res) > 4 {
			TotalNum = 8
		} else {
			TotalNum = int64(2 * len(res))
		}
		j := 0
		i := 0
		for i < int(TotalNum) {
			//delete from customer_0123 where cid=res[0]
			siten[i] = int64(0)
			outsql[i] = "delete from customer_0 where cid=" + strconv.Itoa(res[j])
			i = i + 1
			siten[i] = int64(1)
			outsql[i] = "delete from customer_1 where cid=" + strconv.Itoa(res[j])
			i = i + 1
			j = j + 1
		}
	}
	return TotalNum, outsql, siten
}
```
Insert 部分的语句逻辑更为简单，主要工作集中在分片的判断上，由于 insert 的特殊性，每次仅插入一条语句，因此分片见过只会有一个结果，可以大大减少工作量。进行分片判断->修改语句结构->发送语句到相应站点，逻辑如此。
# 4.3 irpc+client+server（吴）

## 4.3.1 irpc

irpc共分为两部分：负责让各站点iexec开始工作的irpccall和负责站点间传输数据的irpctran。

irpccall需要把一个Txnid传递给相应站点，这样该站点就能凭借Txnid从imeta里获取查询计划树。irpccall中定义了ExecuterCall方法，对应的数据结构、接口定义如下：

```plain
service ExecuterCaller{
rpc ExecuterCall(IrpcCallReq) returns (IrpcStatus) {}
}
message IrpcCallReq {
int64 Txnid = 1;
}
message IrpcStatus {
int64 isSuc = 1;
}
```
接着，需要在同一目录下、package相同的.go文件中实现ExecuterCall方法。

调用iexec中封装好的执行函数RunExecuter,再统计server运行的时间。

```plain
func (s *cserver) ExecuterCall(ctx context.Context, in *IrpcCallReq) (*IrpcStatus, error) {
now := time.Now()
iexec.RunExecuter(in.Txnid)
println(in.Txnid, "time cost:")
println(time.Since(now).Milliseconds(), "ms")
return &IrpcStatus{IsSuc: 1}, nil
}
```
另外，还需要把启动服务、注册服务、监听端口等功能封装好，以便后续由iddb的server调用。

其中，net.Listen()表示监听端口，RegisterExecuterCallerServer表示注册服务。

```go
func RunCallServer() {
// iutilities.LoadAllConfig()
cport = ":" + iutilities.Me.Call
println("rpccall", iutilities.Me.NodeId)
lis, err := net.Listen("tcp", cport)
if err != nil {
log.Fatalf("failed to listen: %v", err)
}
log.Printf("Listening cport %v **************", cport)
s := grpc.NewServer()
RegisterExecuterCallerServer(s, &cserver{})
log.Printf("Server Registering Successfully ***********")
if err := s.Serve(lis); err != nil {
log.Fatalf("failed to serve: %v", err)
}
}
```
最后，将rpccall的调用封装好，调用时只需要说明向哪个地址发送请求，需要执行哪个txnid对应的查询树即可。

```go
func RunCallClient(caddress string, txnid int64) int64 {
defer iutilities.Waitgroup.Done()
println("rpccall client", caddress)
// Set up a connection to the server.
conn, err := grpc.Dial(caddress, grpc.WithInsecure(), grpc.WithBlock())
if err != nil {
log.Fatalf("did not connect: %v", err)
}
defer conn.Close()
c := NewExecuterCallerClient(conn)
ctx, cancel := context.WithTimeout(context.Background(), time.Second*30000)
defer cancel()
irpccallreq := IrpcCallReq{Txnid: txnid}
r, err := c.ExecuterCall(ctx, &irpccallreq)
if err != nil {
log.Fatalf("could not call: %v", err)
}
return r.IsSuc
}
```

irpctran 相对简单，只需要传输一个sql语句即可。

在方案设计时，我们不希望rpc传输数据时还要承载语义信息，同时也不希望接收数据的结点频繁做解码操作，因此我们在发送方就把要传输的数据拼接为sql语句，再由rpc发送至接收方。

另外，我们采取的是PUSH的发送方式，当某个结点做好传输数据的准备时（比如完成了某个选择操作，需要把数据传送给别的站点），就立即执行PushTable方法，将数据传输过去。

```go
servicePushTable {
rpcPushTable(Table) returns (IrpcStatus) {}
}
message Table {
  string createstmt = 1; //create TableName (Col1 varchar(255), Col2 int, Col3 varchar255)
}
```
PushTable方法的实现如下，需要调用itrans内封装好的ExecuteCreateStmt，执行sql语句（包括但不限于create语句）。

```go
func (s *tserver) PushTable(ctx context.Context, in *Table) (*IrpcStatus, error) {
    now := time.Now()
    println("transmission started")
    createstmt := in.Createstmt
    itrans.ExecuteCreateStmt(createstmt)
    // println(in.Createstmt, "time cost:")
    println("transmission time cost=", time.Since(now).Milliseconds(), "ms")
    return &IrpcStatus{IsSuc: 1}, nil
}
```
最后，irpctran的发送和接收也被封装成了两个函数，分别是

```go
func RunTranServer()
```
和
```go
func RunTranClient(taddress string, table Table) int64
```
## 
## 4.3.2 iddbserver

iddb server位于iddb/iddb/server/，使用时允许从命令行读取配置文件的路径。

iddb server做了以下几件事：

1. 获取配置文件的路径（可以使用默认值），可以用同样的代码启动不同的server实例。（比如172服务器上需要启动两个不同的服务）
2. 使用golang原生的并行语法, 启动irpccall的服务和irpctran的服务

为了最大化服务的并行程度，我们使用runtime.GOMAXPROCS(8)将本进程允许的最大核心数设置为8（服务器的核心数），这样就能使两个server并行执行。

在启动完服务后，主进程阻塞，将操作权交给irpctran和irpccall协程（go routine）。

```go
func main() {
    //INIT
    runtime.GOMAXPROCS(8)
    imeta.Connect_etcd()
    for i, v := range os.Args {
        if i == 1 {
            println(i, v)
            iutilities.Configfile = v
            println("iutilities.Configfile= ", iutilities.Configfile)
        }
    }
    iutilities.LoadAllConfig()
    //GET INPUT SQL STATEMENT
    go irpccall.RunCallServer()
    go irpctran.RunTranServer()
    waitgroup.Add(1)
    waitgroup.Wait()
    return
}
```

## 4.3.3 iddbclient

iddb client 位于iddb/iddb/client/

主要功能为：

1. 提供用户交互界面，允许用户从终端中输入sql语句
2. 调用iparser, iqueryanalyzer, iqueryoptimizer处理sql语句
3. 提供内置的测试模块，可以通过命令行参数决定执行查询测试、删除测试和插入测试。
4. 统计每个查询的总时间消耗
5. 读取配置文件、初始化
6. 输出结果

考虑到各站点的工作实际上是并行的，而golang原生语法的协程(go routine)本质上是“假并行，真并发”，同一时间只会有一个协程被执行，除非配置了多处理器参数，因此我需要将最大核心数设置为8

```go
runtime.GOMAXPROCS(8)
```
将查询树保存到etcd后，client会向各站点发送工作信息。如果发现该站点与本次查询无关，则会直接跳过。每次启动一个协程后，都会让waitgroup变量加1，以阻塞主进程。每当该协程结束后，都会让该变量减1。只有所有协程都结束了，主进程才能继续执行。

```go
for i, node := range iutilities.Peers {
     if relsites[i] == 0 {
         continue
     }
      ipaddr = node.IP + ":" + node.Call
      println("call node to work ", node.NodeId)
      iutilities.Waitgroup.Add(1)
      go irpccall.RunCallClient(ipaddr, txnID)
 }
iutilities.Waitgroup.Wait()
```
最后，client输出结果和查询消耗的时间

```go
iexec.PrintResult(plantree, txnID)
println("total time cost: ")
println(time.Since(now).Milliseconds(), " ms"
```



# 4.4 iexec（赵）

该模块负责实际执行由客户端发来的计划树。

在实际实现中，iexec的执行分为如下几步：

1. 客户端会将生成好的计划树存入ectd，由ectd同步到所有的iddb server节点上；同时客户端通过irpc通知各个iddb server要执行的计划树编号。
2. 各个server在接收到计划树编号后，就会从etcd中取出并遍历整个计划树，找出自己可以执行的计划节点。
3. 实际执行时，iexec通过将计划节点的任务转译成sql语句，送入mysql中进行执行，并将执行后的结果存入mysql的临时表中。
4. 在执行完计划节点后，iexec会将刚刚执行完的计划节点状态设置为**已执行完毕**，并更新到etcd中。
5. 之后iexec重新检查etcd中的计划树，找下一个可以执行的节点。
6. 最终当整个计划树的所有节点的状态均为改为**已执行**后，iexec就会结束对当前计划树的检查，并返回给客户端查询完毕的消息。
7. 客户端从计划树根节点的临时表中获取最终的查询结果。

核心代码讲解：

iexec循环检查计划树的函数RunTree

```go
func RunTree(txn_id int64) int64 {
Init()
println("iexecuter init ")
var plan_tree iplan.PlanTree
for {
// TODO:获取 etcd tree
println("txn_id:", txn_id)
plan_tree, err = imeta.Get_Tree(txn_id)
// 检测树是否完全执行完毕
if TreeIsComplete(plan_tree) {
break
}
// 从 tree 中找可执行节点
plan_tree.Print()
execute_id := FindOneNode(plan_tree, plan_tree.Root)
fmt.Println(execute_id)
if execute_id == -1 {
continue
}
var pn *iplan.PlanTreeNode
pn = &plan_tree.Nodes[execute_id]
// 执行某个节点
print("executed node")
println(pn.Nodeid)
print("current node state ")
ExecuteOneNode(pn, plan_tree, txn_id)
var current_node iplan.PlanTreeNode
current_node = plan_tree.Nodes[execute_id]
fmt.Println(current_node)
// TODO:更新 etcd 状态
imeta.Set_Node(txn_id, current_node)
}
println("executed successfully")
return 0
}
```
iexec深度优先遍历计划树，从中找出可执行节点
```go
func FindOneNode(plan_tree iplan.PlanTree, node_id int64) int64 {
Init()
var can_execute_id int64
can_execute_id = -1
// 判断当前节点的状态，如果是 ok，则返回-1
var current_node iplan.PlanTreeNode
current_node = plan_tree.Nodes[node_id]
// Status == 1 若当前节点已执行完毕
if current_node.Status == 1 {
// 则当前节点无法执行，遍历其他子树
return can_execute_id
}
// 否则判断两个子节点的状态，如果都是 ok，则返回当前节点 id
var left_node iplan.PlanTreeNode
var right_node iplan.PlanTreeNode
var can_execute bool
can_execute = true
if current_node.Left != -1 {
left_node = plan_tree.Nodes[current_node.Left]
if left_node.Status == 0 {
can_execute = false
}
}
if current_node.Right != -1 {
right_node = plan_tree.Nodes[current_node.Right]
if right_node.Status == 0 {
can_execute = false
}
}
if can_execute && current_node.Locate == site {
can_execute_id = node_id
return can_execute_id
}
if current_node.Left != -1 {
can_execute_id = FindOneNode(plan_tree, current_node.Left)
}
if can_execute_id != -1 {
return can_execute_id
}
if current_node.Right != -1 {
can_execute_id = FindOneNode(plan_tree, current_node.Right)
}
if can_execute_id != -1 {
return can_execute_id
}
return -1
}
```
iexec实际执行某个计划节点，首先需要根据节点类型，选择不同的执行函数
```go
func ExecuteOneNode(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
Init()
switch {
case plan_node.NodeType == 1 /*Table*/ :
break
case plan_node.NodeType == 2 /*Select or Filter*/ :
ExecuteFilter(plan_node, plan_tree, txn_id)
break
case plan_node.NodeType == 3 /*projection*/ :
ExecuteProjection(plan_node, plan_tree, txn_id)
break
case plan_node.NodeType == 4 /*join*/ :
ExecuteJoin(plan_node, plan_tree, txn_id)
break
case plan_node.NodeType == 5 /*union*/ :
ExecuteUnion(plan_node, plan_tree, txn_id)
break
}
// 检查当前节点的数据是否需要发送给远程 site，如果需要则将数据发送
ExecuteTransmission(plan_node)
}
```
以投影操作为例，iexec首先启动链接mysql，之后根据子节点的临时表生成要执行的sql，并且要将查询到的结果存入新的临时表中，因此生成的mysql sql语句类似于"create table tmp_table_x select cid from tmp_table_x_1 where xxxx"
```go
func ExecuteProjection(plan_node *iplan.PlanTreeNode, plan_tree iplan.PlanTree, txn_id int64) {
mysql := mysql_user + ":" + mysql_passwd + "@tcp(" + mysql_ip_port + ")/" + mysql_db + "?charset=utf8"
db, err := sql.Open("mysql", mysql)
tablename := plan_tree.Nodes[plan_node.Left].TmpTable
query := "create table tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10) + " select " + plan_node.Cols + " from " + tablename
println(query)
stmt, err := db.Prepare(query)
res, err := stmt.Exec()
iutilities.CheckErr(err)
println(res)
plan_node.TmpTable = "tmp_table_" + strconv.FormatInt(txn_id, 10) + "_" + strconv.FormatInt(plan_node.Nodeid, 10)
AddIndex(plan_node)
CleanTmpTable(plan_node.Left, plan_tree)
if !plan_node.TransferFlag {
plan_node.Status = 1
}
}
```
在执行完当前计划节点后，需要考虑的是，当前的数据是否需要发送给其他节点。如果需要，则通过ExecuteTransmission函数进行执发送。
```go
func ExecuteTransmission(plan_node *iplan.PlanTreeNode) {
if plan_node.TransferFlag {
address := getAddress(plan_node)
fmt.Println(address)
// 生成远程建表语句
create_sql := generateCreateQuery(plan_node)
fmt.Println(create_sql)
// 将远程建表语句发送到远程
ExecuteRemoteCreateStmt(address, create_sql)
// 查询临时表中数据，并生成多条 insert 语句
insert_query, issuccess := generateInsertQuery(plan_node)
println("query length is: ", len(insert_query))
if issuccess {
for _, query := range insert_query {
// 将多条 insert 语句发送到远程节点执行
ExecuteRemoteCreateStmt(address, query)
}
}
plan_node.Status = 1
}
}
```
# 4.5 itrans（赵）

该模块负责实际将传输来的数据存储入数据库。具体逻辑是iexec调用irpc发送sql语句，sql语句包括建临时表、插入临时表数据两部分。两部分sql都会通过itrans中的ExecuteCreateStmt进行执行。

```go
func ExecuteCreateStmt(stmt string) int64 {
Init()
println("stmt length is :", len(stmt))
if len(stmt) > 1000 {
println(stmt[0:200])
} else {
println(stmt)
}
stmts, err := db.Prepare(stmt)
if err != nil {
println("could not prepare:", err)
iutilities.CheckErr(err)
}
res, err := stmts.Exec()
if err != nil {
println("could not exec:", err)
}
ins_id, err := res.LastInsertId()
row_aff, err := res.RowsAffected()
println("res: ", ins_id, row_aff)
println(err)
return 0
}
```
5. 实验结果
# 5.1插入、删除测试

下面按照benchmark的要求，演示在iddb上插入和删除数据

清空site0 site1 site2 site3

其中/iddb/data/book/目录下有已经写好的sql脚本

source /home/centos/iddb/data/book/site0_empty.sql

source /home/centos/iddb/data/book/site1_empty.sql

source /home/centos/iddb/data/book/site2_empty.sql

source /home/centos/iddb/data/book/site3_empty.sql

![图片](https://uploader.shimo.im/f/BqNt9Pzuw8t98MLR.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

初始化数据库

![图片](https://uploader.shimo.im/f/zlqu5LUj8amjyZU6.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

执行插入前可以看到customer_0和customer_1均为空表

![图片](https://uploader.shimo.im/f/5deiv7WT565uZhDm.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

执行插入时左上角的site0显示插入成功，右上角的site1显示插入成功

![图片](https://uploader.shimo.im/f/VGhVGwjWfQV2Bf91.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

执行插入后，发现customer_0和customer_1内均有新数据，符合预期

![图片](https://uploader.shimo.im/f/6SBiLdvEJm0Yzrfl.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

执行删除前 先做一次查询获取cid

![图片](https://uploader.shimo.im/f/j5bUtXZhyiXGBmrN.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

接着根据cid生成删除语句，交给site0和site1执行。

5条插入和5条删除语句执行后，数据库为空，符合要求。




# 5.2查询测试

11个query执行结果均正确，总时间和总传输开销如下表所示。benchmark中的11个query的平均查询时间为4.47s，最慢不超过8s。

对于简单的单表查询，如果需要传输的数据量不大，那么执行时间能够压缩在1s之内，如query0和query1。随着传输量提高，总的查询时间也会相应增加。由于在分布式数据库中，主要的开销是连接操作和数据传输，而我们的优化器能够将选择操作尽可能地下推，连接的开销已经尽力减少了，因此网络传输的耗时算是整个系统的瓶颈之处。最慢的查询是query9，其涉及四表连接，同时选择条件筛选出来的记录数比较多，因此有较大的网络传输量，最后也导致比较长的查询时间。

|queries|total time|trans/MB|
|----:|:----|----:|:----|----:|:----|
|0|0.846|0.18652153|
|1|0.701|0.10837936|
|2|3.261|1.13501835|
|3|4.339|1.71819401|
|4|5.723|2.13564587|
|5|5.174|1.913661|
|6|4.509|1.57470322|
|7|5.328|2.93867493|
|8|6.837|2.98496819|
|9|7.796|3.55048943|
|10|4.701|1.70022488|


接下来以sql7为例分析查询树的生成过程。

```sql
select customer.name, customer.rank, orders.quantity
from customer,orders
where customer.id=orders.customer_id
and customer.rank=1
```
首先从ast生成逻辑查询计划树如下图：
![图片](https://uploader.shimo.im/f/UVTB2qBnY1qNPsqD.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

接下来根据分片信息，得出CUSTOMER和ORDERS的所在站点，然后得出物理查询计划如下：

![图片](https://uploader.shimo.im/f/aLA8QsEKCJ8wei8L.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

随后对这个查询树进行选择下推

![图片](https://uploader.shimo.im/f/jdJdOadNSdy2u7TU.png!thumbnail?fileGuid=3fABPbuJUwc1gJOq)

这样就得到了最终的查询计划树。

随后iexec根据这个查询计划树执行查询。iexec会深度优先遍历这个树，找到所有叶子节点，判断节点是否能够执行，即看该节点status是否为1。

在上面的树中，status=1的节点有c.1,c.2,o.1，其他的节点status=0。但由于深度优先遍历时会先遍历到c.1，后遍历到o.1，因此c.1和c.2两个节点会同时在两个站点开始执行，随后select rank=1节点的status也被修改为1。另外，o.2,o.3,o.4虽然status=0，但是他们都需要进行传输，传输完毕后修改status=1。随后检查所有的节点，如果所有的孩子节点status=1，则该节点status=0

随后再次遍历查询计划树，这次找到的执行节点有select （rank=1）, 及U（o.1,o.2），于是site1执行U(o.1,o.2)，site2执行select （rank=1），随后更新status

随后再次遍历查询树，以此类推，每次各个site都去找到当前可以执行的节点并执行，随后更新查询计划树的状态，最后直到所有的节点status都为1，则查询结束。


6. 总结和感想
# 赵泓尧

整体来说，iddb工程基本完成了课程的要求，可以完成简单的分布式查询和插入操作，但是实现非常的基础原始，存在很多需要优化的地方。

通过对分布与并行数据库课程的学习，我主要了解学习了分布式数据库中数据分片和分布式的查询处理、查询优化。同时在实际实现iddb分布式数据库时，接触熟悉了Go语言的开发、irpc、etcd等工具和框架。

以iexec为例，由于对go语言的不熟悉，在执行计划时建立了多个和mysql的链接且未即使清理。导致在长时间测试后，iddb会卡住。

iexec模块中，可以首先优化与mysql建立连接的部分，应该一个server只与mysql建立一个连接。除此以外，整个iddb系统应该实现一套脚本，用来自动建立集群、关闭集群等功能。

# 王大林

本次iddb大作业，我们基本实现了老师的基本要求。这是我第一次尝试使用Go语言进行编程，也是事实上第一次进行多人github项目管理。

我负责的部分是生成查询计划。在这个过程中，我重新温习了数据库系统查询计划的生成过程，一个sql语句是如何从一个字符串一步一步变成ast，逻辑查询计划，物理查询计划。更重要的部分是得到物理查询计划后，如何结合分布式的系统背景进行优化。

我在项目进行的过程中主要遇到了这样几个困难，首先第一个就是对于go语言的不熟悉。因为对interface功能的不了解，导致我在选定parser之后，虽然很早就获得了ast，却苦于看不懂代码，导致无法调用摆在眼前的数据。直到我看到一篇博客，找到调用的函数，这个问题才得以解决。

第二个问题就是对于多表查询的连接节点的逻辑很长时间没有捋清楚，导致我们第一次检查的时候我们只能够跑通SQL6，而不能执行任意的sql语句。这个问题其实静下心来，仔细在纸上盘算一会就可以捋清楚，但之前我一直都是在脑中凭空想象。所以写代码之前一定要把逻辑盘清，在着手写，这样就会更加得心应手。

总的来说，虽然这次大作业遇到了诸多困难，但是对于我自己的能力的提升还是很大的。

# 吴坤尧

本次iddb大作业，我们实现了一个简单的分布式关系数据库，不错地完成了大作业的任务。在这个过程中，我第一次使用golang、第一次写rpc、第一次在多站点环境下部署分布式数据库、第一次使用github作为团队工程管理的工具。

本次大作业，我的收获主要有三部分：分布式数据库理论、分布式数据库实现和团队协作的工程经验。其一是了解了分布式数据库的核心思想，即将数据分片存储在不同站点上，通过并行提高效率。其二是经历了数据库实现，比如查询树（本科课程中只在学过相应的理论和概念），在真正实现时会遇到许多问题：多叉树还是二叉树？自上而下执行还是自下而上执行？传输数据用PUSH还是PULL？优化应该在哪个阶段做？其三是工程经验。我们在中期报告之前进行了两次讨论，讨论氛围比较轻松，进度也比较顺利，所以探讨得比较深入。中期报告时，我们大体确定了工程的框架和分工，而这一框架和分工最终被严格执行。对整体方案设计，我还是非常满意的，架构划分得比较清晰，对于开发者来说是一个simple&work的思路。在实现的过程中，我找到了一些工业级的go语言工程作为参考，按照工程规范写了一些代码，并提醒大家遵守这些规范。

当然，也有一些不足的地方。其一是代码设计时没有考虑到实际执行的情况。比如我们打算先做一棵最简单的查询树，先笛卡尔积，再做选择投影。在实际执行测试时，一个笛卡尔积就会带来1000万条以上的记录插入，这对于本地mysql来说是不可接受的，最终多次导致数据库宕机，浪费了时间。另外，在设计时，我们希望先生成一棵最简单的树，再进行优化，但我们发现优化时想要改变树的结构是相对困难的，最后还是要把部分优化操作集成在生成树的模块。其二是我没有把控好工程进度，导致后期赶进度比较辛苦。本来是打算12.1就能跑一个简单版本，但最后一直到12.15左右才验证了一个query下操作是正确的（数据库里只有少量数据），直到12.25才跑通插入和删除的功能。我反思了一下，也许是计划上设计得太宽泛，导致大家看不到太具体的目标，所以进度一直比较慢。如果有机会，应该多设置一些“check point”，这样就不至于把沉重的负担都集中在最后几天。

总的来说，虽然最后赶工有点辛苦，但收获很多，还是非常值得的。本次大作业中学到的数据库理论知识和积累到的工程经验，对我后续的科研工作有很强的相关性，非常有帮助。



# 水治禹

大作业开始我们组就建立比较好的环境和沟通氛围，从分工来说，走的弯路比较少，绕过了很多坑。每个人的分工都在中期的时候进行了详细的划分，精确位置是在工程的哪一部分的文件夹，我这一块的函数接口、接口的数据输入、数据结构的数据类型和顺序都讨论的比较清楚。整体架构也构建的比较好，因此中期到后期是一个非常自然的推进过程，没有浪费太多时间在对接接口的问题上。

前期我主要负责etcd部分，刚开始还没有走go，因此安装测试了一个单纯的etcd，在后续并没有用到。不过积攒了经验，后续etcd-io已经goreman的安装配置就自然一些，不过也在服务器网络、环境配置、依赖库安装上纠结了一些时间。比如，go get的某些地址是需要翻墙的，不能直接安装，因此我们小组最后采用的是去git上把库的源文件都下载到项目中，需要编译的再进行源码编译的方法。元数据的结构等和大家商量得出后问了需要调用的同学希望的数据形势，前半段就完成接口部分的工作。

我们小组任务推进的比较自然，后期在整体串联的时候卡了几天，在其他同学调试的时候我的工作转向了未完成的插入删除部分，这一部分相较于查询逻辑简单一些，大部分工作不用走查询树，只需要进行语句分析。综合开销和实现复杂度，我选择了正确性能保证且时间开销较少的方式，补充了这一部分的内容。

这次小组合作非常愉快，组员之间都非常好沟通，项目推进也不赶ddl。在中期建立了比较好的沟通基础，后期可能大家投入的时间都不是那么多，所以进度稍有落后。我分析有可能是在查询树按节点刷新这个实现方式是比较origin的，从设想到具体实现的转变可能出了一些问题，中途也有遇到这方面困难没有进度的情况，不过最后能实现出来我觉得大家已经很棒啦！这个的大作业让我注意到对项目管理的方式方法也是我需要学习的，后续我们小组也可以在git上完善我们尚有不足的地方，这次大作业我非常有收获。

