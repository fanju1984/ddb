import pymysql


class Conndb:
    def __init__(
        self, host="localhost", user="root", password="Ddb123$%^", database="db1"
    ):
        self.host = host
        self.user = user
        self.password = password
        self.database = database

        # 打开数据库连接
        self.db = pymysql.connect(
            host=self.host,
            user=self.user,
            password=self.password,
            database=self.database,
        )
        self.cursor = self.db.cursor()

    def read(self, sql):
        # 使用 execute()  方法执行 SQL 查询
        self.cursor.execute(sql)
        data = self.cursor.fetchall()
        return data

    def write(self, sql):
        self.cursor.execute(sql)
        self.db.commit()
        return 200
        # print("write complete")

    def __del__(self):
        # 关闭数据库连接
        self.cursor.close()
        self.db.close()


if __name__ == "__main__":
    conndb = Conndb()
    data = conndb.read("select * from Publisher;")
    print(data)
