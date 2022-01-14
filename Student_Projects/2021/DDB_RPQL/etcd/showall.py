import etcd3
from etcd import Etcd_S
etcd = etcd3.client(host="10.77.70.61", port=2379)
a = etcd.get_all()

for i in a:
    print(i[1].key.decode(), i[0])
