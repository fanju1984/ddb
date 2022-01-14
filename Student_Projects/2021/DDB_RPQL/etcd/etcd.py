import etcd3
import traceback

serverlist = ["10.77.70.61:2379","10.77.70.62:2379","10.77.70.63:2379"]

class Etcd_S:
    def __init__(self, host, port):
        self.etcd = etcd3.client(host=host, port=port)

    def get(self, str):
        try:
            return self.etcd.get(str)
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    return self.etcd.get(str)
                except:
                    pass
            traceback.print_exc()
            return (None, None)

    def get_prefix(self, str):
        try:
            return self.etcd.get_prefix(str)
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    return self.etcd.get_prefix(str)
                except:
                    pass
            traceback.print_exc()
            return (None, None)

    def put(self, str, value):
        try:
            self.etcd.put(str, value)
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    self.etcd.put(str)
                    return
                except:
                    pass
            traceback.print_exc()

    def delete(self, str):
        try:
            self.etcd.delete(str)
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    self.etcd.delete(str)
                    return
                except:
                    pass
            traceback.print_exc()

    def delete_prefix(self, str):
        try:
            self.etcd.delete_prefix(str)
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    self.etcd.delete_prefix(str)
                    return
                except:
                    pass
            traceback.print_exc()
    
    def get_all(self):
        try:
            return self.etcd.get_all()
        except Exception as e:
            for i in serverlist:
                hostport = i.split(":")
                host = hostport[0]
                port = hostport[1]
                self.etcd = etcd3.client(host=host, port=port)
                try:
                    return self.etcd.get_all()
                except:
                    pass
            traceback.print_exc()
            return
            
