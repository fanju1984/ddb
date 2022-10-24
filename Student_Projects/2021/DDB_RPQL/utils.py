from meta import *
import etcd3


def printtable(table_meta: table_meta, data: tuple):
    for field_meta in table_meta.field_meta_list:
        print("| %s " % field_meta.name, end=" ")
    print("|")
    for item in data:
        for i in item:
            print("| %s " % str(i), end=" ")
        print("|")
    print("return: ", len(data), " lines")


def getip():
    import socket

    # 获取本机计算机名称
    hostname = socket.gethostname()
    # 获取本机ip
    ip = socket.gethostbyname(hostname)
    return ip


def getipfromname(etcd, sitename):
    ips = etcd.get("SITES")
    sites = eval(ips[0].decode())
    sitenames = eval(etcd.get("sitenames")[0].decode())
    print(ips)
    print(sites)
    print(sitenames)
    try:
        ip = sitenames[sitename]
        if ip in sites:
            return ip
        else:
            return None
    except:
        return None
