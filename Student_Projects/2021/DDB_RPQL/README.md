# RPQL: a Robust Performant SQL

This is the distributed database course project by the group **Qinliang Xue, Jiawei Liu, Peng Wang**, welcome to join this program, add new features and repair bugs. The name from our three' name: $R_{JR Liu}P_{Peng Wang}QL_{qinliang Xue}$.

If you just want to start from this homework to submit for DDB course, please change another program language.

## Github URL
welcome to star for this program if you like it 😄 !!!

[https://github.com/XueQinliang/DDB_RPQL](https://github.com/XueQinliang/DDB_RPQL)

## Document

Document is available at:

[Midterm Report](./RPQL_midreport.pdf)

[Final Report](./RPQL_finalreport.pdf)

## Framework

- four servers running on three machines
- client can run on any machine
- only for one user to log in, because we have no lock and user design

## Functions and Features

- define sites and sitenames dynamically
- create database
- create table
- create fragment on sites for one table
  - vertical
  - horizontal
  - mixtural(todo: now we support create this kind of table and insert, delete, but not support select on this kind of table)
- insert
  - single data
  - multiple data in one insert language
- load data from a file
- delete
- select
  - not support nested queries
  - can report down node while selecting
- update(todo)
- drop table
- drop database
- switch another database
- add statistic information about table

## How to build environment

if your machines can connect to Internet,

```shell
conda env create -f ddb.yaml	# 直接依据yaml文件创建环境并安装依赖包
pip install -r requirements.txt	# 用于pip导出的
```

if your machines cannot connect to Internet,

```shell
# use a machine can connect, create the conda env and download whl file
mkdir ./your_whl_dir
pip download -r requestments.txt -d ./your_whl_dir
# pack the whl file and upload to your machine, then create the same conda environment on the machine, install the pip package from whl files.
pip install --no-index --find-links=./your_whl_dir -r requirements.txt
```

## How to run

1. scp this program to three machines.

2. before start the ddb, you should cd to the root of project, run `bash net/run.sh`.

3. make sure the etcd and sql service is started, config the etcd and sql information in config.json file.

4. then, cd the server/, run `python servermaster.py ${PORT} ${DBNAME}`. If you want to start multiple servers on one machine, the port **must be different**.

5. you can run `ssh ddb1, ssh ddb2, ssh ddb3` to any server, you should run all the server before run client.

6. cd the client/, run `python clientmaster.py`

7. test sql from the benchmark.sql

8. when you use "select" language, you will see query tree in ./client/tree1.svg.svg file

  ![query tree](./querytree.PNG)

## Detailed examples to run the code

### trans server code to other sites

1. cd /home/centos
2. scp -r DDB_RPQL ddb2:~/
3. scp -r DDB_RPQL ddb3:~/

### detail steps to start four servers

1. log on ddb1(10.77.70.61)
2. split two windows, one to start site1, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`,
   one to start site2, you should `cd DDB_RPQL/server` and run `python servermaster.py 8885 db2`.
3. ssh on ddb2(10.77.70.62)
4. start site3, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`.
5. ssh on ddb3(10.77.70.63)
6. start site4, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`.
7. start client, you should log on ddb1, and `cd DDB_RPQL/client` and run `python clientmaster.py`.

## Instruction of Development

### Add method on service

- If you want to add a method on service, you should add in net/net.proto and the class Servicer in server/servermaster.py. Then run `bash net/run.sh` in root dir.

### Update grpc Server

- On server, you can use ip:port:dbname as key to get the channel with any other servers.
  four sites:
  include [
  ('10.77.70.61','8883','db1'),
  ('10.77.70.61','8885','db2'),
  ('10.77.70.62','8883','db1'),
  ('10.77.70.63','8883','db1')
  ]

```python
# this is a example for client
try:
    conn = grpc.insecure_channel(ip + ":" + port)
    client = net_pb2_grpc.NetServiceStub(channel=conn)
    # print("start")
    response = client.Droptable(net_pb2.SQL(sql=text))
    print(f"message: {response.status} on {sitename}", time.asctime())
except:
    print(f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected", time.asctime())
```

```python
# this is a example for server
def Excute(self, request, context):
    tuple_list = []
    sql = request.sql
    lis = eval(request.sites)
    ... ...
    return net_pb2.TableData(table=(str(tuple_list)))
```

request and response format are defined in net.proto

- By grpc channel to get data, you should use try except because some servers may be inactive.
- etcd use Etcd_S, means safe etcd that with try except.
- db use Conndb, a good class for mysql.

## Contributors

The following people contributed to `RPQL`.

- Qinliang Xue([@XueQinliang](https://github.com/XueQinliang))
- Peng Wang([@LisaWang0306](https://github.com/LisaWang0306))
- Jiawei Liu([@rucjrliu](https://github.com/rucjrliu))
