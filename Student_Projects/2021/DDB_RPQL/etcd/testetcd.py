import etcd3
from etcd import Etcd_S
etcd = etcd3.client(host="10.77.70.61", port=2379)
a = etcd.get_all()

for i in a:
    if i[1].key.decode() == "sitenames" or i[1].key.decode() == "SITES" or i[1].key.decode() == "DB":
        continue
    else:
        print(i[1].key.decode(), i[0])
        etcd.delete(i[1].key.decode())
