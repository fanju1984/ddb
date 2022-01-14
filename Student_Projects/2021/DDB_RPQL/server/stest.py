import etcd3

etcd = etcd3.client(host="10.77.70.61", port=2379)

# print(etcd)  ## <etcd3.client.Etcd3Client object at 0x0000000002EC4240>
# print(etcd.get("SITES"))
# etcd.put("SITES", "")


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

print(getipfromname(etcd, "site1"))
print(getipfromname(etcd, "site2"))
print(getipfromname(etcd, "site3"))
