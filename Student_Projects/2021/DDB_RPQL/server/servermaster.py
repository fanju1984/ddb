#! /usr/bin/env python
# -*- coding: utf-8 -*-
import grpc
import time
import sys
import re
import pickle
import pymysql
from grpc import aio
import asyncio
from concurrent import futures
from concurrent.futures import ThreadPoolExecutor, as_completed
from connect import Conndb
import os
import etcd3
import json

sys.path.append("../")
from net import net_pb2, net_pb2_grpc
from utils import getip
from jr.jr_execute_grpc import dfs
from jr.jr_execute_grpc import jr_execute


with open("../config.json") as f:
    jsondata = json.loads(f.read())
_MYSQL_USER = jsondata["mysqluser"]
_MYSQL_PASSWORD = jsondata["mysqlpassword"]
_ETCD_PORT = jsondata["etcdport"]
_ONE_DAY_IN_SECONDS = 60 * 60 * 24
_IP = getip()
_PORT = ""
_DB = ""
_THIS = ""
_SITES = set()
client = {}
etcd = etcd3.client(host=getip(), port=_ETCD_PORT)


def addclient(ip, port, db):
    print("add:", ip, port, db)
    print("add:", client.keys())
    conn = grpc.insecure_channel(ip + ":" + port)
    clientkey = ip + ":" + port + ":" + db
    client[clientkey] = net_pb2_grpc.NetServiceStub(channel=conn)


def subclient(ip, port, db):
    print("sub:", ip, port, db)
    print("sub:", client.keys())
    if ip + ":" + port + ":" + db in client:
        client.pop(ip + ":" + port + ":" + db)


class servicer(net_pb2_grpc.NetServiceServicer):
    def __init__(self):
        if _DB == "None":
            self.conndb = None
        else:
            self.conndb = Conndb(
                user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB
            )

    def createdatabase(self, request, context):
        dbname = request.dbname + "_" + _PORT
        conn = pymysql.connect(
            host="localhost",
            user=_MYSQL_USER,
            password=_MYSQL_PASSWORD,
            charset="utf8mb4",
        )
        # 创建游标
        cursor = conn.cursor()
        # 创建数据库的sql(如果数据库存在就不创建，防止异常)
        sql = "CREATE DATABASE IF NOT EXISTS " + dbname + ";"
        # 执行创建数据库的sql
        cursor.execute(sql)
        cursor.close()
        # 关闭数据库连接
        conn.close()
        return net_pb2.dbres(res="yes")

    def dropdatabase1(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        if dbname != _DB:
            import os

            if os.path.exists("../db/" + dbname + ".etcdmeta"):
                os.remove("../db/" + dbname + ".etcdmeta")
        else:
            # allmeta = etcd.get_all()
            # pickle.dump(allmeta, "../db/"+_DB+".etcdmeta") # 保存所有
            allmeta = etcd.get_all()
            jr_etcd2txt(allmeta, "../db/" + _DB + ".etcdmeta")  # 写文件
            # allmetalist = [(i[1].key, i[0]) for i in allmeta]
            # metafile = open("../db/"+_DB+".etcdmeta", "wb")
            # pickle.dump(allmetalist, metafile) # 保存所有
            # metafile.close()
        return net_pb2.dbres(res="yes")

    def dropdatabase2(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        if dbname == _DB:
            etcd.delete_prefix("/attrinfo")
            etcd.delete_prefix("/site")
            etcd.delete_prefix("/table")  # 清空现有
        return net_pb2.dbres(res="yes")
        # for i in etcd.get_prefix("/attrinfo"):
        #     print("[Test] To delete", i[1].key, i[0])
        # for i in etcd.get_prefix("/site"):
        #     print("[Test] To delete", i[1].key, i[0])
        # for i in etcd.get_prefix("/table"):
        #     print("[Test] To delete", i[1].key, i[0])

    def dropdatabase3(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        if dbname != _DB:
            self.conndb == None  # 把原来的扔掉
            _DB = "None"
        conn = pymysql.connect(
            host="localhost",
            user=_MYSQL_USER,
            password=_MYSQL_PASSWORD,
            charset="utf8mb4",
        )
        # 创建游标
        cursor = conn.cursor()
        # 删除数据库的sql(如果数据库不存在就不删除，防止异常)
        sql = "DROP DATABASE IF EXISTS " + dbname + ";"
        # 执行创建数据库的sql
        cursor.execute(sql)
        cursor.close()
        # 关闭数据库连接
        conn.close()
        etcdsites = eval(etcd.get("SITES")[0])
        print("delete:", dbname)
        print("before:", etcdsites)
        # newsites = []
        # newdicts = {}
        # thisname = ""
        thisipportdb = ""
        print("ip:", _IP, "port:", _PORT, "db:", _DB)
        for i in etcdsites:
            ip, port, db = i.split(":")
            if ip == _IP and port == _PORT:
                thisipportdb = ":".join([ip, port, _DB])
        return net_pb2.usedbres(res="yes", ipportdb=thisipportdb)

    def usedatabase1(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        print("old_DB:", _DB, "new_DB:", dbname)
        if _DB != "None":
            allmeta = etcd.get_all()
            jr_etcd2txt(allmeta, "../db/" + _DB + ".etcdmeta")  # 写文件

        return net_pb2.dbres(res="yes")

        # allmetalist = [(i[1].key, i[0]) for i in allmeta]
        # metafile = open("../db/"+_DB+".etcdmeta", "wb")
        # pickle.dump(allmetalist, metafile) # 保存所有
        # metafile.close()

    def usedatabase2(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        print("old_DB:", _DB, "new_DB:", dbname)
        if _DB != "None":
            etcd.delete_prefix("/attrinfo")
            etcd.delete_prefix("/site")
            etcd.delete_prefix("/table")  # 清空现有
        return net_pb2.dbres(res="yes")
        # for i in etcd.get_prefix("/attrinfo"):
        #     print("[Test] To delete", i[1].key, i[0])
        # for i in etcd.get_prefix("/site"):
        #     print("[Test] To delete", i[1].key, i[0])
        # for i in etcd.get_prefix("/table"):
        #     print("[Test] To delete", i[1].key, i[0])

    def usedatabase3(self, request, context):
        global _DB
        dbname = request.dbname + "_" + _PORT
        print("old_DB:", _DB, "new_DB:", dbname)
        _DB = dbname
        try:
            self.conndb = Conndb(
                user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB
            )
            if os.path.exists("../db/" + _DB + ".etcdmeta"):
                # metafile = open("../db/"+_DB+".etcdmeta", "rb")
                # allmetalist = pickle.load(metafile) # 加载所有
                # metafile.close()
                allmetalist = jr_txt2etcd("../db/" + _DB + ".etcdmeta")
                for i in allmetalist:
                    if i[0] != "SITES" and i[0] != "sitenames" and i[0] != "DB":
                        try:
                            etcd.put(i[0], i[1])
                        except:
                            pass
                        # print("[Test] To LOAD", i[0], i[1])

            # 修改一大堆东西, 都发送到client端改吧
            # 先修改SITES，按理说有监听，client应该同步修改了
            etcdsites = eval(etcd.get("SITES")[0])
            # newsites = []
            # newdicts = {}
            # thisname = ""
            print("ETCD sites:", etcdsites)
            thisipportdb = ""
            for i in etcdsites:
                ip, port, db = i.split(":")
                if ip == _IP and port == _PORT:
                    thisipportdb = ":".join([ip, port, _DB])
            print("ip port db:", thisipportdb)
            # newdicts[ip+":"+port] = _DB
            # etcd.put("SITES", str(newsites))

            # 再修改sitenames
            # sitenamesdict = eval(etcd.get("sitenames")[0])
            # for i in sitenamesdict:
            #     ip, port, db = sitenamesdict[i].split(":")
            #     db2 = newdicts[ip+":"+port]
            #     sitenamesdict[i] = ":".join(ip, port, db2)
            # etcd.put("SITES", str(sitenamesdict))

            return net_pb2.usedbres(res="yes", ipportdb=thisipportdb)
        except Exception as e:
            print(repr(e))
            return net_pb2.usedbres(res="no", ipportdb="")

    def check(self, request, context):
        username = request.username
        password = request.password
        res = ""
        if username == _MYSQL_USER and password == _MYSQL_PASSWORD:
            res = "yes"
        else:
            res = "no"
        return net_pb2.checkres(res=res)

    def Test(self, request, context):
        d = request.data
        # data = pickle.loads(bytedata)
        # print(data)
        # sdata = pickle.dumps(data)
        return net_pb2.Data1(data=d)

    def Createtable(self, request, context):
        print("in create table")
        sql = request.sql
        print(sql)
        try:
            self.conndb.write(sql)
            return net_pb2.Status(status="create fragment successfully")
        except Exception as e:
            return net_pb2.Status(status=repr(e))

    def Droptable(self, request, context):
        print("in drop table")
        sql = request.sql
        print(sql)
        try:
            self.conndb.write(sql)
            return net_pb2.Status(status="drop table successfully")
        except Exception as e:
            return net_pb2.Status(status=repr(e))

    def Loaddata(self, request, context):
        print("in load", time.asctime())
        table_name = request.table_name
        fragment_columns = request.fragment_columns
        fragment_columns = eval(fragment_columns)
        fragmentlen = len(fragment_columns)
        bytedata = request.data
        data = pickle.loads(bytedata)
        # print(data)
        # table_name = "Book"
        columns = eval(etcd.get("/table/{}/columns".format(table_name))[0])
        trans_funcs = {}
        fragment_columns_type = {}
        count = 0
        for i in columns:
            if i[0] in fragment_columns:
                fragment_columns_type[count] = i[1]
                count += 1
        # print(fragment_columns)
        assert len(fragment_columns_type) == fragmentlen
        strcolumnid = []
        for i in range(fragmentlen):
            if fragment_columns_type[i] == "int":
                trans_funcs[i] = int
            elif fragment_columns_type[i] == "float":
                trans_funcs[i] = float
            else:
                strcolumnid.append(i)
        # print(trans_funcs)
        lendata = len(data)
        # print(lendata)
        size_of_columns = [0 for i in fragment_columns]
        for i in trans_funcs:
            if trans_funcs[i] == int:
                size_of_columns[i] = 4
            elif trans_funcs[i] == float:
                size_of_columns[i] = 8
        for i in range(lendata):
            for j in strcolumnid:
                size_of_columns[j] += len(data[i][j])
        for j in strcolumnid:
            size_of_columns[j] /= lendata
        print(fragment_columns)
        for i in range(fragmentlen):
            # print("fragmentcolumn:", fragment_columns[i])
            columnname = fragment_columns[i]
            size = size_of_columns[i]
            etcd.put(
                "/table/{}/columnsize/{}".format(table_name, columnname), str(size)
            )
        for i in range(lendata):
            # i 目前是单独的一行数据
            # 需要获取一下fragment的信息
            for j in trans_funcs:
                data[i][j] = trans_funcs[j](data[i][j])

            # print(data)
            sql = "insert into " + table_name + " values " + str(tuple(data[i]))
            # print(sql)
            try:
                self.conndb.write(sql)
            except Exception as e:
                return net_pb2.Status(status=repr(e))

        return net_pb2.Status(status="load success")

    def Insertdata(self, request, context):
        print("in insert", time.asctime())
        table_name = request.table_name
        fragment_columns = request.fragment_columns
        fragment_columns = eval(fragment_columns)
        fragmentlen = len(fragment_columns)
        bytedata = request.data
        data = pickle.loads(bytedata)
        # table_name = "Book"
        columns = eval(etcd.get("/table/{}/columns".format(table_name))[0])
        trans_funcs = {}
        fragment_columns_type = {}
        count = 0
        for i in columns:
            if i[0] in fragment_columns:
                fragment_columns_type[count] = i[1]
                count += 1
        # print(fragment_columns)
        assert len(fragment_columns_type) == fragmentlen
        strcolumnid = []
        for i in range(fragmentlen):
            if fragment_columns_type[i] == "int":
                trans_funcs[i] = int
            elif fragment_columns_type[i] == "float":
                trans_funcs[i] = float
            else:
                strcolumnid.append(i)
        print(fragment_columns_type, strcolumnid)

        def get_avg_size_of_columns(table_name, columnname):
            avgsize = etcd.get(
                "/table/{}/columnsize/{}".format(table_name, columnname)
            )[0]
            if not avgsize is None:
                avgsize = eval(avgsize)
                return avgsize
            else:
                return 0

        size_of_columns = [0 for i in fragment_columns]
        print("test")
        for i in trans_funcs:
            if trans_funcs[i] == int:
                size_of_columns[i] = 4
            elif trans_funcs[i] == float:
                size_of_columns[i] = 8
        print("test")
        lendata = len(data)
        print(lendata)
        for i in range(lendata):
            for j in strcolumnid:
                size_of_columns[j] += len(data[i][j])
                print(size_of_columns[j])

        sitenames = eval(etcd.get("sitenames")[0])
        print(sitenames)
        totallen = 0
        for site in sitenames:
            try:
                totallen += eval(
                    etcd.get("/table/{}/lenfragment/{}".format(table_name, site))[0]
                )
            except:
                totallen += 0

        print(fragment_columns)

        for j in strcolumnid:
            print("j:", j)
            avgsize = get_avg_size_of_columns(table_name, fragment_columns[j])
            print(avgsize, size_of_columns[j])
            size_of_columns[j] = (avgsize * totallen + size_of_columns[j]) / (
                totallen + lendata
            )
            print(avgsize, size_of_columns[j])
        print(fragment_columns)
        for i in range(fragmentlen):
            print("fragmentcolumn:", fragment_columns[i])
            print("size column:", size_of_columns)
            columnname = fragment_columns[i]
            size = size_of_columns[i]
            etcd.put(
                "/table/{}/columnsize/{}".format(table_name, columnname), str(size)
            )

        # print(lendata)
        for i in range(lendata):
            # i 目前是单独的一行数据
            # 需要获取一下fragment的信息
            for j in trans_funcs:
                data[i][j] = trans_funcs[j](data[i][j])
            # print(data)
            sql = "insert into " + table_name + " values " + str(tuple(data[i]))
            # print(sql)
            try:
                self.conndb.write(sql)
            except Exception as e:
                return net_pb2.DataReturn(status=repr(e), data=bytedata)

        bytedata = pickle.dumps(data)
        return net_pb2.DataReturn(status="insert data successfully", data=bytedata)

    def Deletedata(self, request, context):
        print("in delete", time.asctime())
        sql = request.sql
        print(sql)
        sql2 = re.sub("delete from", "select * from", sql)
        print(sql2)
        data = self.conndb.read(sql2)
        print(data, "is deleted")
        bytedata = pickle.dumps(data)
        try:
            self.conndb.write(sql)
            return net_pb2.DataReturn(status="delete data successfully", data=bytedata)
        except Exception as e:
            return net_pb2.DataReturn(status=repr(e), data=bytedata)

    def SimpleSelect(self, request, context):
        # 根据查询条件返回主键的值
        print("in SimpleSelect", time.asctime())
        sql = request.sql
        sql = re.sub("delete", "select *", sql)
        print(sql)
        table_name = re.findall("select\s+\*\s+from\s+(.*?)[\s.;]+?", sql)[0]
        columns = etcd.get("/table/" + table_name + "/columns")[0]
        try:
            columns = eval(columns)
        except:
            print("something in etcd is wrong!")
        key = ""
        for i in columns:
            if len(i) == 3 and i[2] == "key":
                key = i[0]
        sql2 = re.sub("\*", key, sql)
        data = self.conndb.read(sql2)
        print(data, "key of deletedata")
        bytedata = pickle.dumps(data)
        return net_pb2.SimpleSelectReturn(
            status="to delete data's key", key=key, data=bytedata
        )

    def Excute(self, request, context):
        tuple_list = []
        sql = request.sql
        lis = eval(request.sites)
        # print(lis)
        for i in lis:
            ip, port, db = i
            # print(ip, port, db)
            # print(ip == _IP)
            # print(port == _PORT)
            if ip == _IP and port == _PORT:
                print(ip, port)
                print("get local")
                data = self.conndb.read(sql)
                tuple_list += list(data)
            else:
                print(ip, port)
                print("get remote")
                response = client[":".join([ip, port, db])].Excute(
                    net_pb2.SQLTree(sql=sql, sites=str([i]))
                )
                data = eval(response.table)
                tuple_list += list(data)

        return net_pb2.TableData(table=(str(tuple_list)))

    def jr_grpc_test(self, request, context):  # jr测试grpc远程过程调用
        print("OK" + str(request.site))
        th = request.th
        site = request.site
        site2ipd = [
            None,
            "10.77.70.61:8883:db1",
            "10.77.70.61:8885:db2",
            "10.77.70.62:8883:db1",
            "10.77.70.63:8883:db1",
        ]
        global client

        print("Site" + str(site + 1) + " " + str(th))
        if th == 4:
            return net_pb2.ret_jr_grpc_test(res=str(site + 1))

        nex_site = (site + 1) % 4
        try:
            net_conn = client[site2ipd[nex_site + 1]]
        except KeyError as e:
            print("WARNING! " + str(e) + " NOT found")
        else:
            response = net_conn.jr_grpc_test(
                net_pb2.para_jr_grpc_test(th=th + 1, site=nex_site)
            )
            res = response.res
            print("Site" + str(site + 1) + " return from Site" + res)

        return net_pb2.ret_jr_grpc_test(res=str(site + 1))

    def grpc_dfs(self, request, context):
        # get paras from request
        query_no = request.query_no
        node_no = request.node_no
        str_nodes = request.str_nodes

        # call dfs local
        # 把servermaster.py里的 conndb、clients给dfs
        global client
        dfs_node_no, str_columns, str_values, trans_vol = dfs(
            query_no, node_no, str_nodes, self.conndb, client
        )

        # return
        return net_pb2.ret_grpc_dfs(
            dfs_node_no=dfs_node_no,
            str_columns=str_columns,
            str_values=str_values,
            trans_vol=trans_vol,
        )

    def start_jr(self, request, context):
        print("Now Start!")
        str_nodes = request.str_nodes
        # print(str_nodes)
        from jr.jr_execute_grpc import jr_execute
        from jr.jr_execute_grpc import str2nodes

        # nodes = pickle.loads(etcd.get('/tree')[0])
        nodes = str2nodes(str_nodes)
        sitenames = eval(etcd.get("sitenames")[0])
        print(sitenames)
        root_site = None
        global _IP, _PORT, _DB
        for i in list(sitenames.items()):
            if i[1] == ":".join([_IP, _PORT, _DB]):
                root_site = i[0]
                break
        global client
        print("start_jr:" + root_site)
        restr = str(jr_execute(23, nodes, root_site, self.conndb, client))
        # print("complete")
        # 垃圾回收
        # for i in list(sitenames.items()):
        #     key = i[1]
        #     response = client[key].temp_GC(net_pb2.para_temp_GC(para="0"))
        #     if response.res == "GC":
        #         print("GC")
        #     else:
        #         print("drop temp table error!")
        return net_pb2.ret_start_jr(
            res=restr
        )  # eval解析出(columns,data,trans_vol) data=tuple(tuple)


def serve():
    # with open("../config.json") as f:
    #     config = json.load(f)
    ips = etcd.get("SITES")

    def watch_callback(event):
        global _SITES
        ips = etcd.get("SITES")
        sites = set(eval(ips[0].decode()))
        addsites = sites - _SITES
        subsites = _SITES - sites
        print(sites, _SITES)
        _SITES = sites
        for s in addsites:
            addclient(*s.split(":"))
            print("add site:", s)
        for s in subsites:
            subclient(*s.split(":"))
            print("sub site:", s)
        print("final:", client.keys())

    watch_id = etcd.add_watch_prefix_callback("SITES", watch_callback)
    sites = ""
    # print(ips)
    if ips[0] == b"" or ips[0] == b"[]" or ips[0] == None:
        sites = [":".join([_IP, _PORT, _DB])]
        etcd.put("SITES", str(sites))
        _SITES.add(sites[0])
    else:
        sites = eval(ips[0].decode())
        newsite = ":".join([_IP, _PORT, _DB])
        if newsite not in sites:
            sites.append(newsite)
        etcd.put("SITES", str(sites))
        _SITES.add(newsite)

    grpcServer = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    net_pb2_grpc.add_NetServiceServicer_to_server(servicer(), grpcServer)
    grpcServer.add_insecure_port(_IP + ":" + _PORT)
    # recieve watch events via callback function
    grpcServer.start()
    print("start server on " + _IP + ":" + _PORT)

    # ### jr_grpc_test
    # this_site = jr_siteofme()
    # print('_THIS site' + str(this_site))
    # root_site = 1
    # if this_site == root_site:
    #     res = servicer.jr_grpc_test(0, this_site - 1)
    #     print('return from Site' + res)
    # else:
    #     print('_THIS is NOT root_site')
    ###

    try:
        time.sleep(_ONE_DAY_IN_SECONDS)
    except KeyboardInterrupt:
        ips = etcd.get("SITES")
        sites = eval(ips[0].decode())
        thissite = ":".join([_IP, _PORT, _DB])
        sites.remove(thissite)
        etcd.put("SITES", str(sites))
        print("test")

        # etcd flush disk
        allmeta = etcd.get_all()
        allmetalist = [(i[1].key, i[0]) for i in allmeta]
        """
        metafile = open("../db/"+_DB+".etcdmeta", "wb")
        pickle.dump(allmetalist, metafile) # 保存所有
        metafile.close()
        """
        jr_etcd2txt(allmeta, "../db/" + _DB + ".etcdmeta")
        # etcd.delete_prefix("/attrinfo")
        # etcd.delete_prefix("/site")
        # etcd.delete_prefix("/table") # 清空现有

        grpcServer.stop(0)


def jr_txt2etcd(filename):  # [(key,value),(),...]
    print("【!!!】text2etcd")

    try:
        filein = open(filename, "r", encoding="utf-8")
        text = filein.readlines()
        filein.close()
    except:
        print("【!!!】text2etcd can NOT read")
        return []

    etcd_res = []
    for i in range(0, len(text) // 2):
        etcd_res.append((text[i * 2].strip("\n"), text[i * 2 + 1].strip("\n")))

    print("【!!!】text2etcd FINISHED")
    return etcd_res


def jr_etcd2txt(etcd_all, filename):
    print("【!!!】etcd2text")
    fileout = open(filename, "w", encoding="utf-8")
    for i in etcd_all:
        try:
            out_str = str(i[1].key.decode()) + "\n" + str(i[0].decode()) + "\n"
            fileout.write(out_str)
        except Exception as e:
            print("ERROR! " + str(i[1].key.decode()) + " " + repr(e))
            continue
    fileout.close()
    print("【!!!】etcd2text FINISHED")


def jr_exit(self, request, context):
    print("【EXIT】")
    dbname = request.dbname

    # etcd flush disk
    allmeta = etcd.get_all()
    jr_etcd2txt(allmeta, "../db/" + _DB + ".etcdmeta")
    etcd.delete_prefix("/attrinfo")
    etcd.delete_prefix("/site")
    etcd.delete_prefix("/table")  # 清空现有

    print("【EXIT】" + dbname)
    self.stop(0)
    return net_pb2.ret_jr_exit(dbname=dbname)


def jr_siteofme():
    global _THIS
    site2ipd = {
        "10.77.70.61:8883:db1": 1,
        "10.77.70.61:8885:db2": 2,
        "10.77.70.62:8883:db1": 3,
        "10.77.70.63:8883:db1": 4,
    }
    if not site2ipd.__contains__(_THIS):
        return 0
    return site2ipd[_THIS]


if __name__ == "__main__":
    if sys.argv[1] != None:
        _PORT = sys.argv[1]
    try:
        _DB = sys.argv[2] + "_" + _PORT
    except:
        _DB = "None"
    _THIS = ":".join([_IP, _PORT, _DB])
    serve()
    # serve()
    # serve()
