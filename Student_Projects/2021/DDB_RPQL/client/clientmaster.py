#! /usr/bin/env python
# -*- coding: utf-8 -*-
import enum
from google.protobuf import message
import grpc
import sys
import re
import time
import etcd3
import pickle
import asyncio
from grpc import aio
from concurrent.futures import ThreadPoolExecutor, as_completed

try:
    import gnureadline as readline
except ImportError:
    import readline

sys.path.append("../")

from utils import *
from net import net_pb2, net_pb2_grpc
from etcd.etcd import Etcd_S
from jr.jr_execute_grpc import update_sitename

# IP1 = "10.46.234.251"
# IP2 = "10.46.234.251"

# PORT1 = "8883"
# PORT2 = "8885"

# _HOST = "localhost"
# _PORT = "8883"

_DB = "None"

# etcd = etcd3.client(host=getip(), port=2379)
etcd = Etcd_S(host=getip(), port=2379)

# conn1 = grpc.insecure_channel("localhost:8883")
# client1 = net_pb2_grpc.NetServiceStub(channel=conn1)

# publisher_meta = table_meta(
#    "Publisher",
#    [
#        field_meta("id", "int"),
#        field_meta("name", "char(100)"),
#        field_meta("nation", "char(3)"),
#    ],
#    [fragment_meta(IP1, PORT1, "db1"), fragment_meta(IP2, PORT2, "db2")],
# )


def run():
    global _DB
    sites = etcd.get("SITES")[0]
    if sites in [b"", b"[]", b"{}", None]:
        print("no server started!")
        exit(0)
    else:
        sites = eval(sites)
        dbs = set()
        for site in sites:
            ip, port, db = site.split(":")
            db = db.split("_")[0]
            dbs.add(db)
        if len(dbs) == 1:
            _DB = list(dbs)[0]
        else:
            print("the state of server is not consistent! Something must be wrong!")
    while True:
        table = etcd.get("/table")[0]
        # print('table',table)
        text = input("({})> ".format(_DB))
        if text == "quit" or text == "exit":
            break
        if re.match("create table", text):  # 加个统计信息
            # 写入etcd
            table_name = re.findall("create table (.*?)\s*\(.*?", text)[0]
            table = etcd.get("/table")[0]
            # print('table',table)
            if table in [b"", b"[]", b"{}", None]:
                table = [table_name]
            else:
                table = eval(table)
                if table_name not in table:
                    table.append(table_name)
                else:
                    print("the table already exists")
                    continue
            etcd.put("/table", str(table))
            columnlist = [
                i.strip().split(" ")
                for i in re.findall("\((.*?)\)\s*;", text)[0].split(",")
            ]
            etcd.put("/table/" + table_name + "/columns", str(columnlist))
            print("create table successfully")
            # etcd.put("/table/" + table_name + "/fragmentnum", "0")

        elif re.match("create database", text):
            dbname = re.findall("create database\s+(.*?);", text)
            sitenames = etcd.get("sitenames")[0]
            if sitenames in [b"", b"[]", b"{}", None]:
                print("please define sitenames first!")
                continue
            else:
                sitenames = eval(sitenames)
            sites = eval(etcd.get("SITES")[0])
            dblist = etcd.get("DB")[0]
            if dblist in [b"", b"[]", b"{}", None]:
                dblist = []
            else:
                dblist = eval(dblist)
            if len(dbname) == 1:
                dbname = dbname[0]
                if dbname == "None":
                    print("database name can't be 'None'")
                    continue
                if dbname in dblist:
                    print("This database has existed!")
                    continue
                for name in sitenames:
                    ip, port, db = sitenames[name].split(":")
                    try:
                        conn = grpc.insecure_channel(ip + ":" + port)
                        client = net_pb2_grpc.NetServiceStub(channel=conn)
                        # print("start")
                        response = client.createdatabase(
                            net_pb2.para_dbname(dbname=dbname)
                        )
                        if response.res == "yes":
                            print("create database on {} successfully!".format(name))
                    except:
                        print(f"{ip}:{port} cannot be connected", time.asctime())

                # 写db
                if dblist == []:
                    dblist = [dbname]
                    # print("[Test] DB", dblist)
                    etcd.put("DB", str(dblist))
                else:
                    dblist.append(dbname)
                    # print("[Test] DB", dblist)
                    etcd.put("DB", str(dblist))

            else:
                print("parse error!")
                continue

        elif re.match("drop database", text):
            dbname = re.findall("drop\s+database\s+(.*?);", text)
            sitenames = eval(etcd.get("sitenames")[0])
            sites = eval(etcd.get("SITES")[0])
            newSITES = []
            newsitenames = {}
            if len(dbname) == 1:
                dbname = dbname[0]
                if dbname == "None":
                    print("'None' database not exist")
                    continue
                for name in sitenames:
                    ip, port, db = sitenames[name].split(":")
                    try:
                        conn = grpc.insecure_channel(ip + ":" + port)
                        client = net_pb2_grpc.NetServiceStub(channel=conn)
                        # print("start")
                        print(dbname)
                        response = client.dropdatabase1(
                            net_pb2.para_dbname(dbname=dbname)
                        )
                        print(response.res)
                    except:
                        print(f"{ip}:{port} cannot be connected", time.asctime())

                for name in sitenames:
                    ip, port, db = sitenames[name].split(":")
                    try:
                        conn = grpc.insecure_channel(ip + ":" + port)
                        client = net_pb2_grpc.NetServiceStub(channel=conn)
                        # print("start")
                        print(dbname)
                        response = client.dropdatabase2(
                            net_pb2.para_dbname(dbname=dbname)
                        )
                        print(response.res)
                    except:
                        print(f"{ip}:{port} cannot be connected", time.asctime())

                for name in sitenames:
                    ip, port, db = sitenames[name].split(":")
                    try:
                        conn = grpc.insecure_channel(ip + ":" + port)
                        client = net_pb2_grpc.NetServiceStub(channel=conn)
                        # print("start")
                        print(dbname)
                        response = client.dropdatabase3(
                            net_pb2.para_dbname(dbname=dbname)
                        )
                        ipportdb = response.ipportdb
                        print(response.res)
                        print(ipportdb)
                        if response.res == "yes" and ipportdb != "":
                            print("drop database on {} successfully!".format(name))
                            # todo 没写完
                            newSITES.append(ipportdb)
                            newsitenames[name] = ipportdb
                    except:
                        print(f"{ip}:{port} cannot be connected", time.asctime())

                if _DB == dbname:
                    _DB = "None"
                rplace = True
                for i in newSITES:
                    if i == "":
                        rplace = False
                        break
                if rplace:
                    etcd.put("SITES", str(newSITES))
                    etcd.put("sitenames", str(newsitenames))
                    print("[change] SITES", newSITES)
                    print("[change] sitenames", newsitenames)
                # if dbname != _DB:
                #     import os
                #     os.remove("../db/"+dbname+".etcdmeta")
                # else:
                #     allmeta = etcd.get_all()
                #     pickle.dump(allmeta, "../db/"+_DB+".etcdmeta") # 保存所有
                #     etcd.delete_prefix("/attrinfo")
                #     etcd.delete_prefix("/site")
                #     etcd.delete_prefix("/table") # 清空现有
                # 写db
                dblist = etcd.get("DB")[0]
                if dblist in [b"", b"[]", b"{}", None]:
                    print("Error! There is no this DB!")
                else:
                    dblist = eval(dblist)
                    if dbname in dblist:
                        dblist.remove(dbname)
                        # print("[Test] DB", dblist)
                        etcd.put("DB", str(dblist))
                    else:
                        print("Error! There is no this DB!")
            else:
                print("parse error!")
                continue

        elif re.match("use", text):
            dbname = re.findall("use\s+(.*?);", text)
            sitenames = eval(etcd.get("sitenames")[0])
            sites = eval(etcd.get("SITES")[0])
            dblist = etcd.get("DB")[0]
            if dblist != None:
                dblist = eval(dblist)
            else:
                print("there is no database!")
                continue
            if len(dbname) == 1:
                dbname = dbname[0]
                if not re.match("[A-Za-z_]+[0-9]*$", dbname):
                    print("perror")
                    continue
                if dbname == "None":
                    print("'None' database not exist")
                    continue
                if dbname == _DB:
                    continue
                if dbname != _DB:
                    newSITES = []
                    newsitenames = {}
                    for name in sitenames:
                        ip, port, db = sitenames[name].split(":")
                        print(ip, port, db)
                        try:
                            conn = grpc.insecure_channel(ip + ":" + port)
                            client = net_pb2_grpc.NetServiceStub(channel=conn)
                            # print("start")
                            print("send", dbname)
                            response = client.usedatabase1(
                                net_pb2.para_dbname(dbname=dbname)
                            )
                            print(response.res)
                        except:
                            print(f"{ip}:{port} cannot be connected", time.asctime())

                    for name in sitenames:
                        ip, port, db = sitenames[name].split(":")
                        print(ip, port, db)
                        try:
                            conn = grpc.insecure_channel(ip + ":" + port)
                            client = net_pb2_grpc.NetServiceStub(channel=conn)
                            # print("start")
                            print("send", dbname)
                            response = client.usedatabase2(
                                net_pb2.para_dbname(dbname=dbname)
                            )
                            print(response.res)
                        except:
                            print(f"{ip}:{port} cannot be connected", time.asctime())

                    for name in sitenames:
                        ip, port, db = sitenames[name].split(":")
                        print(ip, port, db)
                        try:
                            conn = grpc.insecure_channel(ip + ":" + port)
                            client = net_pb2_grpc.NetServiceStub(channel=conn)
                            # print("start")
                            print("send", dbname)
                            response = client.usedatabase3(
                                net_pb2.para_dbname(dbname=dbname)
                            )
                            ipportdb = response.ipportdb
                            print(response.res)
                            print(ipportdb)
                            if response.res == "yes" and ipportdb != "":
                                print("use database on {} successfully!".format(name))
                                # todo 没写完
                                newSITES.append(ipportdb)
                                newsitenames[name] = ipportdb
                        except:
                            print(f"{ip}:{port} cannot be connected", time.asctime())

                    print("add newsites:", newSITES)
                    rplace = True
                    for i in newSITES:
                        if i == "":
                            rplace = False
                            break
                    if rplace:
                        _DB = dbname
                        etcd.put("SITES", str(newSITES))
                        etcd.put("sitenames", str(newsitenames))
                        print("[change] SITES", newSITES)
                        print("[change] sitenames", newsitenames)
            else:
                print("parse error!")
                continue

        elif re.match("quit", text):
            print("quit")
            dbname = _DB
            sites = eval(etcd.get("SITES")[0])
            print(sites)
            for i in sites:
                ip, port, db = i.split(":")
                print(ip, port)
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                # print("start")
                print("exit while", dbname)
                try:
                    response = client.jr_exit(net_pb2.para_jr_exit(dbname=dbname))
                except:
                    print("test")
                    pass
                print(ip + ":" + port + ": exit finished", dbname)
                exit(0)

        elif re.match("clean", text):  # 清除etcd
            etcd.delete_prefix("/attrinfo")
            etcd.delete_prefix("/site")
            etcd.delete_prefix("/table")  # 清空现有

        elif re.match("show databases", text):
            db = etcd.get("DB")[0]
            if db in [b"", b"[]", b"{}", None]:
                print("None")
            else:
                for i in eval(db):
                    print(i)

        elif re.match("show tables", text):
            try:
                table = eval(etcd.get("/table")[0])
            except:
                print("tables:\nNone")
                continue
            print("tables:")
            for i in table:
                print(i)
                if re.findall("with\s+fragment", text):
                    fragments = etcd.get_prefix("/table/" + i + "/fragment")
                    for f in fragments:
                        fragment = eval(f[0])
                        import json

                        print(json.dumps(fragment, indent=4, sort_keys=False))

        elif re.match("show sites", text):
            sitenames = etcd.get("sitenames")[0]
            if sitenames not in [b"", b"[]", b"{}", None]:
                sitenames = eval(sitenames)
            else:
                print("None")
                continue
            print(sitenames)
            print("sites:")
            for i in sitenames:
                ip, port, db = sitenames[i].split(":")
                print(i, "ip:", ip, "port:", port, "db:", db)
                if re.findall("with\s+fragment", text):
                    fragments = etcd.get_prefix("/site/" + i + "/fragment")
                    for f in fragments:
                        fragment = eval(f[0])
                        import json

                        print(json.dumps(fragment, indent=4, sort_keys=False))

        elif re.match("create fragment", text):
            # create fragment Publisher.1(*) where id < 205000 on site1;
            # create fragment Publisher.2(id, nation) where id < 205000 on site2;
            # create fragment Book.2(id, title) where id < 203 on site2;
            tokens = re.findall("create fragment (.*?)\((.*?)\)", text)[0]
            conditions = re.findall("where (.*?)\s+on\s+.*?", text)
            if conditions == []:
                conditions = ""
            else:
                conditions = conditions[0]
            site = re.findall("\s+on\s+site\s*=\s*['\"](.*?)['\"]\s*;", text)[0]
            # print(site)
            table_name = tokens[0]
            columnlist = [i.strip() for i in tokens[1].split(",")]
            sitenames = eval(etcd.get("sitenames")[0])
            sites = eval(etcd.get("SITES")[0])
            # reverse_sitenames = {v:k for k,v in sitenames.items()}
            # if site not in reverse_sitenames:
            #     print("this sitename is not defined!")
            #     continue
            this_sitename = site
            # print(sitename, site)
            if site in sitenames:
                site = sitenames[site]
            if site not in sites:
                print(site)
                print("this site is not in the cluster")
                continue
            if columnlist[0] == "*":
                try:
                    columnlist = [
                        i[0]
                        for i in eval(
                            etcd.get("/table/" + table_name + "/columns")[0].decode()
                        )
                    ]
                except:
                    print("this table is not in database")
                    continue
            fragment = {
                "columns": columnlist,
                "conditions": conditions,
                "sitename": this_sitename,
                "site": site,
                "size": 0,
            }
            # print(fragment)
            tablecolumns = eval(etcd.get("/table/" + table_name + "/columns")[0])
            fragcolumns = []
            for i in tablecolumns:
                if i[0] in columnlist:
                    fragcolumns.append(i)
            createsql = (
                "create table "
                + table_name
                + "("
                + ",".join([" ".join(i) for i in fragcolumns])
                + ");"
            )
            # 创建表
            ip, port, db = site.split(":")
            # if ip == getip():
            #     ip = "localhost"
            print(this_sitename, ip, port)
            # ip = "localhost"
            # port = "8883"

            etcd.put(
                "/table/" + table_name + "/fragment/" + this_sitename, str(fragment)
            )
            etcd.put(
                "/site/" + this_sitename + "/fragment/" + table_name, str(fragment)
            )
            # num = etcd.get("/table/" + table_name + "/fragmentnum")[0]
            # etcd.put("/table/" + table_name + "/fragmentnum", str(int(num) + 1))
            try:
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                # print("start")
                response = client.Createtable(net_pb2.SQL(sql=createsql))
                print(response.status)
            except:
                print(f"{ip}:{port} cannot be connected", time.asctime())

        elif re.match("load data infile", text):
            t1 = time.time()
            # load data infile "/home/centos/DDB_RPQL/data/book.tsv" into table Book;
            path = re.findall("[\"|'](.*?)[\"|']", text)[0]
            table_name = re.findall("into\s+table\s+(.*?)\s*;", text)[0]
            fragment = []
            # fragmentnum = int(
            #     etcd.get("/table/" + table_name + "/fragmentnum")[0].decode()
            # )
            # connections = []
            data = []
            fragment_columns = []
            for i in etcd.get_prefix("/table/" + table_name + "/fragment"):
                # print((etcd.get("/table/" + table_name + "/fragment/" + str(i + 1))[0]))
                fragitem = eval(i[0])
                fragment.append(fragitem)
                print(i[0])
                ip, port, db = fragitem["site"].split(":")
                # if ip == getip():
                #     ip = "localhost"
                # print(ip, port)
                # conn = grpc.aio.insecure_channel(ip + ":" + port)
                # client = net_pb2_grpc.NetServiceStub(channel=conn)
                # connections.append(client)
                data.append([])
                fragment_columns.append(fragitem["columns"])
            # print(fragment_columns)
            count = 0
            with open(path) as f:
                tablecolumns = [
                    i[0] for i in eval(etcd.get("/table/" + table_name + "/columns")[0])
                ]
                complist = []
                fragmentlen = len(fragment)
                for j in range(fragmentlen):
                    if fragment[j]["conditions"] == "":
                        complist.append([])
                    else:
                        conditionlist = fragment[j]["conditions"].split("and")
                        complexcomp = []
                        for comp in conditionlist:
                            # print(comp)
                            tempcomp = True
                            left, op, right = re.findall(
                                "(.+?)\s*([>=<]+)\s*(.+)", comp
                            )[0]
                            left = left.strip(" ")
                            right = right.strip(" '\"")
                            op = op.strip(" ")
                            # print(left, op, right)
                            complexcomp.append((left, op, right))
                        complist.append(complexcomp)
                # print(complist)
                for i in f.readlines():
                    i = i.strip("\n")
                    value = i.split("\t")
                    row = dict(zip(tablecolumns, value))
                    # print(row)
                    for j in range(fragmentlen):
                        if fragment[j]["conditions"] == "":
                            data[j].append(
                                [row[name] for name in fragment[j]["columns"]]
                            )
                        else:
                            # print(fragment[j]["conditions"])
                            # conditionlist = fragment[j]["conditions"].split("and")
                            complexcomp = complist[j]
                            # print(conditionlist)
                            ifinsert = True
                            # print(complexcomp)
                            for left, op, right in complexcomp:
                                # print(comp)
                                tempcomp = True
                                # left, op, right = re.findall(
                                #     "(.+?)\s*([>=<]+)\s*(.+)", comp
                                # )[0]
                                # right = right.strip("'\"")
                                # print(row[left], op, right)
                                if op == "=":
                                    tempcomp = row[left] == right
                                elif op == ">":
                                    tempcomp = row[left] > right
                                elif op == "<":
                                    tempcomp = row[left] < right
                                elif op == ">=":
                                    tempcomp = row[left] >= right
                                elif op == "<=":
                                    tempcomp = row[left] <= right
                                elif op == "<>":
                                    tempcomp = row[left] != right
                                if tempcomp == False:
                                    ifinsert = False
                                    break
                            if ifinsert:
                                data[j].append(
                                    [row[name] for name in fragment[j]["columns"]]
                                )
                                # print(
                                #     j,
                                #     "insert row",
                                #     [row[name] for name in fragment[j]["columns"]],
                                # )
                    count += 1
                    # print(count)
            print(count)

            def tmp(i, ip, port, table_name, fragment_columns, bytedata):
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                try:
                    response = client.Loaddata(
                        net_pb2.LoadParams(
                            table_name=(table_name),
                            fragment_columns=(str(fragment_columns[i])),
                            data=bytedata,
                        )
                    )
                    return (response.status, ip, port)
                except:
                    print(
                        f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                        time.asctime(),
                    )
                    return ("something is wrong", ip, port)

            sitenames = eval(etcd.get("sitenames")[0])
            namesites = {}
            for i in sitenames:
                ip, port, db = sitenames[i].split(":")
                namesites[ip + ":" + port] = i

            # loop = asyncio.get_event_loop()
            tasks = []
            with ThreadPoolExecutor(max_workers=5) as t:
                for i in range(len(fragment)):
                    subdata = data[i]
                    len_of_fragment = len(subdata)
                    # reverse_sitenames = {v:k for k,v in sitenames.items()}

                    etcd.put(
                        "/table/{}/lenfragment/{}".format(
                            table_name, fragment[i]["sitename"]
                        ),
                        str(len_of_fragment),
                    )
                    bytedata = pickle.dumps(subdata)
                    ip, port, db = fragment[i]["site"].split(":")
                    print("start", i)
                    # tasks.append(tmp(i, ip, port, table_name, fragment_columns, bytedata))
                    task = t.submit(
                        tmp, i, ip, port, table_name, fragment_columns, bytedata
                    )
                    tasks.append(task)

                for future in as_completed(tasks):
                    message, ip, port = future.result()
                    if ip + ":" + port in namesites:
                        sitename = namesites[ip + ":" + port]
                        print(f"message: {message} on {sitename}", time.asctime())
                    else:
                        print(f"message: {message} on {ip}:{port}", time.asctime())
                # print(response.data)
                # print(pickle.loads(response.data))
                # print("%d success" % (i + 1))
            # loop.run_until_complete(asyncio.wait(tasks))
            t2 = time.time()
            print("time:", t2 - t1)

        elif re.match("drop table", text):
            table_name = re.findall("drop\s+table\s+(.*?)\s*;", text)[0]
            table = etcd.get_prefix("/table/" + table_name)
            tableexist = False
            for i in table:
                tableexist = True
            if tableexist == False:
                print(f"there is no this table:{table_name}")
                continue
            sitenames = eval(etcd.get("sitenames")[0])
            namesites = {}
            for i in sitenames:
                ip, port, db = sitenames[i].split(":")
                namesites[ip + ":" + port] = i

            for i in etcd.get_prefix("/table/" + table_name + "/fragment"):
                fragitem = eval(i[0])
                ip, port, db = fragitem["site"].split(":")
                sitename = ip + ":" + port
                if ip + ":" + port in namesites:
                    sitename = namesites[ip + ":" + port]
                print("involved site:", sitename)

                try:
                    conn = grpc.insecure_channel(ip + ":" + port)
                    client = net_pb2_grpc.NetServiceStub(channel=conn)
                    # print("start")
                    response = client.Droptable(net_pb2.SQL(sql=text))
                    print(f"message: {response.status} on {sitename}", time.asctime())
                except:
                    print(
                        f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                        time.asctime(),
                    )

            etcd.delete_prefix("/table/" + table_name)
            tables = etcd.get("/table")[0]
            tables = eval(tables)
            tables.remove(table_name)
            etcd.put("/table", str(tables))

            attrinfo = etcd.get_prefix("/attrinfo/")
            for i in attrinfo:
                attr = eval(i[0])["attr"]
                if attr.split(".")[0] == table_name:
                    etcd.delete(i[1].key)

            siteinfo = etcd.get_prefix("/site/")
            for i in siteinfo:
                site_table_name = i[1].key.decode().split("/")[-1]
                if site_table_name == table_name:
                    etcd.delete(i[1].key)

            print("drop table and fragment meta successfully", time.asctime())

        elif re.match("insert into", text):
            # insert into Customer(id, name, rank) values (300002,'Xiaohong', 1), (300001, 'Xiaoming', 1);
            # path = re.findall("[\"|'](.*?)[\"|']", text)[0]
            table = re.findall("insert\s+into\s+(.*?)s*values.*?;", text)[0].strip()
            table_name = re.sub("\(.*?\)", "", table).strip()
            table_columns = re.findall("\((.*?)\)", table)
            # print(table_name, table_columns)
            if table_columns != []:
                table_columns = table_columns[0].strip().split(",")
            else:
                table_columns = [
                    i[0] for i in eval(etcd.get("/table/" + table_name + "/columns")[0])
                ]
            data = re.findall("values\s*(.*?)\s*;", text)[0]
            insertdata = [eval(i.strip()) for i in re.findall("(\(.*?\))", data)]
            fragment = []
            data = []
            fragment_columns = []
            for i in etcd.get_prefix("/table/" + table_name + "/fragment"):
                fragitem = eval(i[0])
                fragment.append(fragitem)
                print(i[0])
                ip, port, db = fragitem["site"].split(":")
                data.append([])
                fragment_columns.append(fragitem["columns"])

            count = 0
            # with open(path) as f:
            tablecolumns = [
                i[0] for i in eval(etcd.get("/table/" + table_name + "/columns")[0])
            ]
            complist = []
            fragmentlen = len(fragment)
            for j in range(fragmentlen):
                if fragment[j]["conditions"] == "":
                    complist.append([])
                else:
                    conditionlist = fragment[j]["conditions"].split("and")
                    complexcomp = []
                    for comp in conditionlist:
                        # print(comp)
                        tempcomp = True
                        left, op, right = re.findall("(.+?)\s*([>=<]+)\s*(.+)", comp)[0]
                        left = left.strip(" ")
                        right = right.strip(" '\"")
                        op = op.strip(" ")
                        # print(left, op, right)
                        complexcomp.append((left, op, right))
                    complist.append(complexcomp)
            # print(complist)
            # for i in f.readlines():
            #     i = i.strip("\n")
            #     value = i.split("\t")
            for value in insertdata:
                row = dict(zip(tablecolumns, value))
                # print(row)
                for j in range(fragmentlen):
                    if fragment[j]["conditions"] == "":
                        data[j].append([row[name] for name in fragment[j]["columns"]])
                    else:
                        # print(fragment[j]["conditions"])
                        # conditionlist = fragment[j]["conditions"].split("and")
                        complexcomp = complist[j]
                        # print(conditionlist)
                        ifinsert = True
                        # print(complexcomp)

                        for left, op, right in complexcomp:
                            # print(comp)
                            row[left] = str(row[left])
                            tempcomp = True
                            # left, op, right = re.findall(
                            #     "(.+?)\s*([>=<]+)\s*(.+)", comp
                            # )[0]
                            # right = right.strip("'\"")
                            # print(row[left], op, right)
                            if op == "=":
                                tempcomp = row[left] == right
                            elif op == ">":
                                tempcomp = row[left] > right
                            elif op == "<":
                                tempcomp = row[left] < right
                            elif op == ">=":
                                tempcomp = row[left] >= right
                            elif op == "<=":
                                tempcomp = row[left] <= right
                            elif op == "<>":
                                tempcomp = row[left] != right
                            if tempcomp == False:
                                ifinsert = False
                                break
                        if ifinsert:
                            data[j].append(
                                [row[name] for name in fragment[j]["columns"]]
                            )
                            # print(
                            #     j,
                            #     "insert row",
                            #     [row[name] for name in fragment[j]["columns"]],
                            # )
                count += 1
                # print(count)
            # print(count)

            def tmp(i, ip, port, table_name, fragment_columns, bytedata):
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                try:
                    response = client.Insertdata(
                        net_pb2.LoadParams(
                            table_name=(table_name),
                            fragment_columns=(str(fragment_columns[i])),
                            data=bytedata,
                        )
                    )
                    print(f"insert data:{pickle.loads(response.data)}")
                    return (response.status, ip, port)
                except:
                    print(
                        f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                        time.asctime(),
                    )
                    return ("something is wrong", ip, port)

            sitenames = eval(etcd.get("sitenames")[0])
            namesites = {}
            for i in sitenames:
                ip, port, db = sitenames[i].split(":")
                namesites[ip + ":" + port] = i

            # loop = asyncio.get_event_loop()
            tasks = []
            with ThreadPoolExecutor(max_workers=5) as t:
                for i in range(len(fragment)):
                    subdata = data[i]
                    len_of_fragment = len(subdata)
                    # reverse_sitenames = {v:k for k,v in sitenames.items()}

                    etcd.put(
                        "/table/{}/lenfragment/{}".format(
                            table_name, fragment[i]["sitename"]
                        ),
                        str(len_of_fragment),
                    )
                    bytedata = pickle.dumps(subdata)
                    ip, port, db = fragment[i]["site"].split(":")
                    print("start", i)
                    # tasks.append(tmp(i, ip, port, table_name, fragment_columns, bytedata))
                    task = t.submit(
                        tmp, i, ip, port, table_name, fragment_columns, bytedata
                    )
                    tasks.append(task)

                for future in as_completed(tasks):
                    message, ip, port = future.result()
                    if ip + ":" + port in namesites:
                        sitename = namesites[ip + ":" + port]
                        print(f"message: {message} on {sitename}", time.asctime())
                    else:
                        print(f"message: {message} on {ip}:{port}", time.asctime())
                # print(response.data)
                # print(pickle.loads(response.data))
                # print("%d success" % (i + 1))
            # loop.run_until_complete(asyncio.wait(tasks))
            # t2 = time.time()
            # print("time:", t2 - t1)

        elif re.match("delete from", text):
            # delete from Customer where name='L. Morrison' AND rank=1;
            # delete from Customer where name='M. Hart' AND rank=3;
            table_name, conditionlist = re.findall(
                "delete\s+from\s+(.*?)(\s+where\s+.*?){0,1}\s*;", text
            )[0]
            columnlist = []
            if conditionlist != "":
                conditionlist = conditionlist.replace(" where ", "")
                conditionlist = re.split(" and | or | AND | OR ", conditionlist)
                columnlist = [re.split(">|<|=", i)[0].strip() for i in conditionlist]
            print(conditionlist)
            table = etcd.get_prefix("/table/" + table_name)
            tableexist = False
            for i in table:
                tableexist = True
            if tableexist == False:
                print(f"there is no this table:{table_name}")
                continue
            sitenames = eval(etcd.get("sitenames")[0])
            namesites = {}
            for i in sitenames:
                ip, port, db = sitenames[i].split(":")
                namesites[ip + ":" + port] = i

            vertical = False
            fragmentlist = []
            table_columns = eval(etcd.get("/table/" + table_name + "/columns")[0])
            print(table_columns)
            for i in etcd.get_prefix("/table/" + table_name + "/fragment"):
                fragitem = eval(i[0])
                fragmentlist.append(fragitem)
                print(fragitem)
                if len(fragitem["columns"]) != len(table_columns):
                    # 简单考虑要么水平分片，要么垂直分片
                    vertical = True

            clientlist = []
            keyset = set()
            keyslist = []
            keycolumn = ""
            for fragitem in fragmentlist:
                ip, port, db = fragitem["site"].split(":")
                sitename = ip + ":" + port
                if ip + ":" + port in namesites:
                    sitename = namesites[ip + ":" + port]
                columns = fragitem["columns"]
                print("involved site:", sitename)
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                if vertical and columnlist != []:
                    # 如果是垂直分片，且有选择条件
                    try:
                        clientlist.append(client)
                        frag_where_columns = []
                        for i, column in enumerate(columnlist):
                            if column in columns:
                                # 说明这个条件属于这个分片上
                                frag_where_columns.append(conditionlist[i])
                        delete_sql = "delete from " + table_name
                        if frag_where_columns != []:
                            delete_sql += (
                                " where " + " and ".join(frag_where_columns) + ";"
                            )
                        # if conditionlist != "" and set(columnlist) - set(columns) != set(): #如果条件中涉及到分块上没有的列
                        #     continue
                        print(delete_sql)
                        response = client.SimpleSelect(net_pb2.SQL(sql=delete_sql))
                        data = pickle.loads(response.data)
                        # print(data)
                        keyslist.append(data)
                        keycolumn = response.key
                        # print(f"data keys:{pickle.loads(response.data)}")
                    except:
                        print(
                            f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                            time.asctime(),
                        )
                else:
                    try:
                        print("start")
                        response = client.Deletedata(net_pb2.SQL(sql=text))
                        data = pickle.loads(response.data)
                        # print(f"delete data:{data}")
                        print(f"delete rows:{len(data)}")
                        print(
                            f"message: {response.status} on {sitename}", time.asctime()
                        )
                    except:
                        print(
                            f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                            time.asctime(),
                        )

            if vertical and columnlist != []:
                for i, keys in enumerate(keyslist):
                    # print(i, len(keys))
                    if i == 0:
                        keyset = set(keys)
                    else:
                        keyset = keyset & set(keys)

                # 修改sql语句
                # print(keyset)
                delete_keys = [str(key[0]) for key in keyset]
                # print(delete_keys)
                delete_sql = (
                    "delete from "
                    + table_name
                    + " where "
                    + keycolumn
                    + " in "
                    + str("(" + ",".join(delete_keys) + ")")
                    + ";"
                )
                # print(delete_sql)
                for i, client in enumerate(clientlist):
                    try:
                        print("start")
                        response = client.Deletedata(net_pb2.SQL(sql=delete_sql))
                        data = pickle.loads(response.data)
                        # print(f"delete data:{data}")
                        print(f"delete rows:{len(data)}")
                        print(
                            f"message: {response.status} on {sitename}", time.asctime()
                        )
                    except:
                        print(
                            f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected",
                            time.asctime(),
                        )

                    # print("start")
                    # response = client.Deletedata(net_pb2.SQL(sql=text))
                    # print(f"delete data:{pickle.loads(response.data)}")
                    # print(f"message: {response.status} on {sitename}", time.asctime())
                # except:
                #   print(f"there is no table {table_name} on {ip}:{port} or {ip}:{port} cannot be connected", time.asctime())

        elif re.match("define", text):
            # 定义站点名称
            # define site1 10.77.70.61 8883
            command = text.split(" ")
            sitename = command[1]
            sip = command[2]
            sport = command[3]
            ips = etcd.get("SITES")
            sites = eval(ips[0].decode())
            # print(sites)
            status = False
            for i in sites:
                ip, port, db = i.split(":")
                if ip == sip and port == sport:
                    sitenames = etcd.get("sitenames")[0]
                    if sitenames in [
                        b"",
                        b"[]",
                        b"{}",
                        None,
                    ]:  # sitenames is empty, a new sitenames
                        print("sitenames is empty, a new sitenames")
                        sitenames = {sitename: ip + ":" + port + ":" + db}
                        # print(sitenames)
                        etcd.put("sitenames", str(sitenames))
                        status = True
                    else:
                        sitenames = eval(sitenames)
                        old_sitename = None
                        for j in list(sitenames.items()):
                            if ip + ":" + port + ":" + db == j[1]:
                                old_sitename = j
                                break
                        if (
                            old_sitename != None
                        ):  # sitenames contain the site, update_sitename
                            print("sitenames contain the site, update_sitename")
                            print(old_sitename[0], sitename)
                            update_sitename(old_sitename[0], sitename)
                            sitenames = eval(etcd.get("sitenames")[0])
                            # print(sitenames)
                            etcd.put("sitenames", str(sitenames))
                            status = True
                        else:  # sitenames not contain the site, add
                            print("sitenames not contain the site, add")
                            sitenames[sitename] = ip + ":" + port + ":" + db
                            # print(sitenames)
                            etcd.put("sitenames", str(sitenames))
                            status = True
                    break
            if status:
                print("define successfully")
            else:
                print("ip port not in the cluster")
            pass

        elif re.match("add info", text):
            infos = text.split(" ")
            writeinfo = {"attr": infos[2], "type": infos[3], "numbers": infos[4:]}
            etcd.put("/attrinfo/" + infos[2], str(writeinfo))
            if writeinfo["attr"].split(".")[0] not in eval(etcd.get("/table")[0]):
                print("table", attr.split(".")[0], "not exist")
            else:
                etcd.put("/attrinfo/" + infos[2], str(writeinfo))
                print("add info successfully!")
            # pass

        elif re.match("select", text):
            time1 = time.time()
            # 解析sql语句
            import create_tree_bak1 as tree

            nodes = tree.create_a_tree(text)
            print("node length:", len(nodes))
            # nodes = pickle.loads(etcd.get('/tree')[0])
            root_node_site = str(nodes[-1].site)
            sitenames = eval(etcd.get("sitenames")[0])
            # print(sitenames)
            # print(root_node_site)
            ipportdb = sitenames[root_node_site]
            ip, port, db = ipportdb.split(":")
            # print(nodes)
            # nodes2 = pickle.loads(etcd.get('/tree')[0])

            from jr.jr_execute_grpc import nodes2str

            str_nodes = nodes2str(nodes)
            # str_nodes = "test sql"
            # root_node_site = "RUC"
            # ip = "10.77.70.61"
            # port = "8883"
            # str_nodes = nodes2str(nodes)

            try:
                print("send request to " + root_node_site + ": " + ip + ":" + port)
                conn = grpc.insecure_channel(ip + ":" + port)
                client = net_pb2_grpc.NetServiceStub(channel=conn)
                # print(str_nodes)
                response = client.start_jr(net_pb2.para_start_jr(str_nodes=str_nodes))
                res_list = eval(response.res)
                time2 = time.time()
                print("cost time: ", time2 - time1, " sec")
                print("result table column: ", res_list[0])
                # print("result: ", res_list[1])
                print("result tuple len: ", len(res_list[1]))
                if res_list[0] == []:
                    print("broken site:", res_list[2])
                    print("one site may be wrong! Lost connection!")
                else:
                    print("result trans vol: ", res_list[2])
            except Exception as e:
                print(repr(e))
                print("broken site:", root_node_site)
                print("one site may be wrong! Lost connection!")

        else:
            pass
        # elif re.match("temp gc", text):
        #    sitenames = eval(etcd.get('sitenames')[0])
        #
        #    for i in list(sitenames.items()):
        #        print(i)
        #        ip, port, db = i[1].split(":")
        #        conn = grpc.insecure_channel(ip + ":" + port)
        #        client = net_pb2_grpc.NetServiceStub(channel=conn)
        #        try:
        #            response = client.temp_GC(net_pb2.para_temp_GC(para="0"))
        #            if response.res == "GC":
        #                print("GC")
        #            else:
        #                print("drop temp table error!")
        #        except Exception as e:
        #            print(repr(e))
        #            print(i[0], "site can't not connected!")

        # elif re.match("test", text):
        #    ip = "10.77.70.61"
        #    port = "8883"
        #    conn = grpc.insecure_channel(ip + ":" + port)
        #    client = net_pb2_grpc.NetServiceStub(channel=conn)
        #    print("client")
        #    response = client.Test(net_pb2.Data(data="test"))
        #    print(response.data)
        # else:
        #    pass
        # table_name = re.findall("from\s+([_A-Za-z]+[0-9]*)\s*.*?;", text)
        # # print(table_name)
        # fragment = eval(etcd.get(table_name[0])[0])
        # lis = []
        # for request in fragment:
        #     ip = etcd.get(request["ip"])[0].decode()
        #     port = etcd.get(request["port"])[0].decode()
        #     dbname = etcd.get(request["db"])[0].decode()
        #     lis.append((ip, port, dbname))
        # # print(str(lis))
        # conn = grpc.insecure_channel(_HOST + ":" + _PORT)
        # client = net_pb2_grpc.NetServiceStub(channel=conn)
        # response = client.Excute(net_pb2.SQLTree(sql=text, sites=str(lis)))
        # printtable(publisher_meta, eval(response.table))


if __name__ == "__main__":
    run()
