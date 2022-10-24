class table_meta:
    def __init__(self, name, field_meta_list, fragment_meta_list):
        self.name = name
        self.field_meta_list = field_meta_list
        self.fragment_meta_list = fragment_meta_list


class field_meta:
    def __init__(self, name, attrtype, iskey=False, nullable=True):
        self.name = name
        self.attrtype = attrtype
        self.iskey = iskey
        self.nullable = nullable


class fragment_meta:
    def __init__(self, ip, port, db):
        self.ip = ip
        self.port = port
        self.db = db
