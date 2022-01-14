import grpc
import os
import pickle
import etcd3

etcd = etcd3.client(host="10.77.70.61", port=2379)

_DB = "test22_8885"
if os.path.exists("../db/"+_DB+".etcdmeta"):
    metafile = open("../db/"+_DB+".etcdmeta", "rb")
    allmetalist = pickle.load(metafile) # 加载所有
    metafile.close()
    for i in allmetalist:
        if i[0] != "SITES" and i[0] != "sitenames" and i[0] != "DB":
                print(i[0].decode(), i[1].decode(), type(i[0]))