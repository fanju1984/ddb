# DDB_RPQL
This is the distributed database course project by the group **Qinliang Xue, Jiawei Liu, Peng Wang**
# URL
https://github.com/XueQinliang/DDB_RPQL
## How to run
1. before start the ddb, you should cd to the root of project, run `bash net/run.sh`

2. then edit the config.json to set the sites

3. make sure the etcd service is started

4. then, cd the server/, run `python servermaster.py ${PORT} ${DBNAME}`

5. you can run `ssh ddb1, ssh ddb2, ssh ddb3` to any server, you should run all the server before run client.

6. cd the client/, run `python clientmaster.py`

# How to trans server code to other sites

1. cd /home/centos

2. scp -r DDB_RPQL ddb2:~/

3. scp -r DDB_RPQL ddb3:~/

# detail steps to start four servers

1. log on ddb1(10.77.70.61)

2. split two windows, one to start site1, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`,
one to start site2, you should `cd DDB_RPQL/server` and run `python servermaster.py 8885 db2`.

3. ssh on ddb2(10.77.70.62)

4. start site3, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`.

5. ssh on ddb3(10.77.70.63)

6. start site4, you should `cd DDB_RPQL/server` and run `python servermaster.py 8883 db1`.

7. start client, you should log on ddb1, and `cd DDB_RPQL/client` and run `python clientmaster.py`.

##  Instruction of Development
- If you want to add a method on service, you should add in net/net.proto and the class Servicer in server/servermaster.py. Then run `bash net/run.sh` in root dir.
### Server
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

The following people contributed to `RPQL`.

- Qinliang Xue([@XueQinliang](https://github.com/XueQinliang))
- Peng Wang([@LisaWang0306](https://github.com/LisaWang0306))
- Jiawei Liu([@rucjrliu](https://github.com/rucjrliu))

