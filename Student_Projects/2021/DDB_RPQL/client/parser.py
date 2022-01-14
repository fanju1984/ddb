import sqlparse


def sql2ast(sql: str):
    return sqlparse.parse(sql)


def ast2sql(ast):
    return "".join([i.value for i in ast.flatten()])


if __name__ == "__main__":
    # 例子
    sql = "select * from t;"
    ast = sql2ast(sql)
    sql2 = ast2sql(ast[0])
    print(sql2)
