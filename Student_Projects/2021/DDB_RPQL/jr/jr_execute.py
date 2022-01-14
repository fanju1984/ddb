import sys
from typing import List
from concurrent.futures import ThreadPoolExecutor, as_completed
import grpc
import pickle
#import structures
#from structures import Node

sys.path.append("../")
from net import net_pb2, net_pb2_grpc
from server.connect import Conndb
import etcd3
from etcd.etcd import Etcd_S
import client.structures
from client.structures import Node

etcd = etcd3.client(host="10.77.70.61", port=2379)

def NodeEqual(node_x, node_y):
    eq = True
    
    eq = eq and node_x.id == node_y.id
    eq = eq and node_x.type == node_y.type
    eq = eq and node_x.parent == node_y.parent
    eq = eq and node_x.children == node_y.children
    eq = eq and node_x.tables == node_y.tables
    eq = eq and node_x.site == node_y.site
    eq = eq and node_x.size == node_y.size
    eq = eq and node_x.if_valid == node_y.if_valid
    eq = eq and node_x.columns == node_y.columns
    eq = eq and node_x.f_id == node_y.f_id
    eq = eq and node_x.f_name == node_y.f_name
    eq = eq and node_x.select_condi == node_y.select_condi
    eq = eq and node_x.projection == node_y.projection
    eq = eq and node_x.join == node_y.join
    eq = eq and node_x.top == node_y.top

    return eq

def nodesEqual(nodes_x, nodes_y):
    if len(nodes_x) != len(nodes_y):
        return False
    eq = True
    for i in range(0, len(nodes_x)):
        eq = eq and NodeEqual(nodes_x[i], nodes_y[i])
    return eq

def Node2dict(node_x):
    dict_x = {}
    dict_x['id'] = node_x.id                   #node id, start from 0
    dict_x['type'] = node_x.type                   #type fragment, select, projection, join, union
    dict_x['parent'] = node_x.parent             #the parent node id, default -1
    dict_x['children'] = node_x.children         #the children node ids, defalut []
    dict_x['tables'] = node_x.tables             #table names in this node
    dict_x['site'] = node_x.site                 #which site this node is in
    dict_x['size'] = node_x.size                 #total bytes of data in this node
    dict_x['if_valid'] = node_x.if_valid          #if this node has been pruned
    dict_x['columns'] = node_x.columns
    dict_x['f_id'] = node_x.f_id                  #if type is 'fragment', the fragment id
    dict_x['f_name'] = node_x.f_name            
    dict_x['select_condi'] = node_x.select_condi       #if type is 'select', the select condition
    dict_x['projection'] = node_x.projection      #if type is 'projection', the project attributes
    dict_x['join'] = node_x.join                 #if type is join, the join condition
    dict_x['top'] = node_x.top            #if type is 'fragment', correspond id for join
    return dict_x

def dict2Node(dict_x):
    node_x = Node(dict_x['id'],
        dict_x['type'],
        dict_x['parent'],
        dict_x['children'],
        dict_x['tables'],
        dict_x['site'],
        dict_x['size'],
        dict_x['columns'],
        dict_x['if_valid'],
        dict_x['f_id'],
        dict_x['f_name'],
        dict_x['select_condi'],
        dict_x['projection'],
        dict_x['join'],
        dict_x['top']
    )
    return node_x

def str2nodes(str_x):
    nodes_x = eval(str_x)
    for i in range(0, len(nodes_x)):
        nodes_x[i] = dict2Node(nodes_x[i])
    return nodes_x

def nodes2str(nodes_x):
    str_x = []
    for i in nodes_x:
        str_x.append(Node2dict(i))
    return str(str_x)

nodes = []
nodes.append(Node(0,'fragment',
                    parent = 12,
                    children = [],
                    tables=['Customer'],
                    site = 1,
                    size=0,
                    columns=[],
                    f_id=7))
nodes.append(Node(1,'fragment',
                    parent = 13,
                    children = [],
                    tables=['Customer'],
                    site = 1,
                    size=0,
                    columns=[],
                    f_id=7))
nodes.append(Node(2,'fragment',
                    parent = 14,
                    children = [],
                    tables=['Customer'],
                    site = 1,
                    size=0,
                    columns=[],
                    f_id=7))
nodes.append(Node(3,'fragment',
                    parent = 15,
                    children = [],
                    tables=['Customer'],
                    site = 1,
                    size=0,
                    columns=[],
                    f_id=7))
nodes.append(Node(4,'fragment',
                    parent = 8,
                    children = [],
                    tables=['Orders'],
                    site = 1,
                    size=0,
                    columns=[],
                    f_id=9))
nodes.append(Node(5,'fragment',
                    parent = 9,
                    children = [],
                    tables=['Orders'],
                    site = 2,
                    size=0,
                    columns=[],
                    f_id=10))
nodes.append(Node(6,'fragment',
                    parent = 10,
                    children = [],
                    tables=['Orders'],
                    site = 3,
                    size=0,
                    columns=[],
                    f_id=11))
nodes.append(Node(7,'fragment',
                    parent = 11,
                    children = [],
                    tables=['Orders'],
                    site = 4,
                    size=0,
                    columns=[],
                    f_id=12))
nodes.append(Node(8,'projection',
                    parent = 12,
                    children = [4],
                    tables=['Orders'],
                    site = 1,
                    size=0,
                    columns=[],
                    projection=['Orders.customer_id','Orders.quantity']))
nodes.append(Node(9,'projection',
                    parent = 13,
                    children = [5],
                    tables=['Orders'],
                    site = 2,
                    size=0,
                    columns=[],
                    projection=['Orders.customer_id','Orders.quantity']))
nodes.append(Node(10,'projection',
                    parent = 14,
                    children = [6],
                    tables=['Orders'],
                    site = 3,
                    size=0,
                    columns=[],
                    projection=['Orders.customer_id','Orders.quantity']))
nodes.append(Node(11,'projection',
                    parent = 15,
                    children = [7],
                    tables=['Orders'],
                    site = 4,
                    size=0,
                    columns=[],
                    projection=['Orders.customer_id','Orders.quantity']))
nodes.append(Node(12,'join',
                    parent = 16,
                    children = [0,8],
                    tables=['Customer','Orders'],
                    site = 1,
                    size=0,
                    columns=[],
                    join=['Customer.id','Orders.customer_id']))
nodes.append(Node(13,'join',
                    parent = 17,
                    children = [1,9],
                    tables=['Customer','Orders'],
                    site = 2,
                    size=0,
                    columns=[],
                    join=['Customer.id','Orders.customer_id']))
nodes.append(Node(14,'join',
                    parent = 18,
                    children = [2,10],
                    tables=['Customer','Orders'],
                    site = 3,
                    size=0,
                    columns=[],
                    join=['Customer.id','Orders.customer_id']))
nodes.append(Node(15,'join',
                    parent = 19,
                    children = [3,11],
                    tables=['Customer','Orders'],
                    site = 4,
                    size=0,
                    columns=[],
                    join=['Customer.id','Orders.customer_id']))
nodes.append(Node(16,'projection',
                    parent = 20,
                    children = [12],
                    tables=['Customer','Orders'],
                    site = 1,
                    size=0,
                    columns=[],
                    projection=['Customer.name','Orders.quantity']))
nodes.append(Node(17,'projection',
                    parent = 20,
                    children = [13],
                    tables=['Customer','Orders'],
                    site = 2,
                    size=0,
                    columns=[],
                    projection=['Customer.name','Orders.quantity']))
nodes.append(Node(18,'projection',
                    parent = 20,
                    children = [14],
                    tables=['Customer','Orders'],
                    site = 3,
                    size=0,
                    columns=[],
                    projection=['Customer.name','Orders.quantity']))
nodes.append(Node(19,'projection',
                    parent = 20,
                    children = [15],
                    tables=['Customer','Orders'],
                    site = 4,
                    size=0,
                    columns=[],
                    projection=['Customer.name','Orders.quantity']))
nodes.append(Node(20,'union',
                    parent = -1,
                    children = [16,17,18,19],
                    tables=['Customer','Orders'],
                    site = 2,
                    size=0,
                    columns=[]))
### example above

def table_column_from_pj(pj):
    tables = []
    columns = []
    for i in pj:
        point_loc = i.find('.')
        tables.append(i[0 : point_loc])
        columns.append(i[point_loc+1 : len(i)])
    return tables, columns

def table_column_type(table, column):
    columns = eval(etcd.get('/table/' + table + '/columns')[0])
    for i in columns:
        if i[0] == column:
            return table, column, i[1]

def fragment_columns(table, site):
    return eval(etcd.get('/table/' + table + '/fragment/' + str(site))[0])['columns']

def sql_create(query_no, node_no, columns):
    sql = 'create table Q' + str(query_no) + '_N' + str(node_no) + ' ('
    sql += columns[0][0] + '_' + columns[0][1] + ' ' + columns[0][2]
    for i in range(1, len(columns)):
        sql += ', ' + columns[i][0] + '_' + columns[i][1] + ' ' + columns[i][2]
    sql += ');'
    return sql

def valueij(valueij):
    if str(type(valueij)).find('str') >= 0:
        return "'" + valueij + "'"
    else:
        return str(valueij)

def value_tuple(value):
    sql = '(' + valueij(value[0])
    for i in range(1, len(value)):
        sql += ', ' + valueij(value[i])
    sql += ')'
    return sql

def sql_insert(query_no, node_no, columns, values):
    if values == []:
        return ''
    
    sql = 'insert into Q' + str(query_no) + '_N' + str(node_no) + '('
    sql += columns[0][0] + '_' + columns[0][1]
    for i in range(1, len(columns)):
        sql += ', ' + columns[i][0] + '_' + columns[i][1]
    
    sql += ') values'
    sql += value_tuple(values[0])
    for i in range(1, len(values)):
        sql += ', ' + value_tuple(values[i])
    
    sql += ';'
    return sql

def sql_select_fragment(columns):
    sql = 'select ' + columns[0][1]
    for i in range(1, len(columns)):
        sql += ', ' + columns[i][1]
    sql += ' from ' + columns[0][0]
    sql += ';'
    return sql

def sql_select_star(query_no, node_no):
    return 'select * from Q' + str(query_no) + '_N' + str(node_no) + ';' 

def sql_select(query_no, node_no, columns, select_condi):
    ss = sql_select_star(query_no, node_no)
    sql = ss[0 : len(ss)-1]

    sql += ' where Q' + str(query_no) + '_N' + str(node_no) + '.' + columns[0][0] + '_' + columns[0][1]
    sql += ' ' + select_condi[1] + ' ' + select_condi[2]
    sql += ';'
    return sql

def sql_project(query_no, node_no, columns):
    sql = 'select ' + columns[0][0] + '_' + columns[0][1]
    for i in range(1, len(columns)):
        sql += ', ' + columns[i][0] + '_' + columns[i][1]
    sql += ' from Q' + str(query_no) + '_N' + str(node_no) + ';'
    return sql

def sql_union(query_no, children):
    sss = sql_select_star(query_no, children[0])
    sql = sss[0 : len(sss)-1]
    for i in range(1, len(children)):
        sss = sql_select_star(query_no, children[i])
        sql += ' UNION ALL ' + sss[0 : len(sss)-1]
    sql += ';'
    return sql

def sql_join(query_no, nodes_no, columns):
    sql = 'select * from Q' + str(query_no) + '_N' + str(nodes_no[0])
    sql += ', Q' + str(query_no) + '_N' + str(nodes_no[1])
    sql += ' where Q' + str(query_no) + '_N' + str(nodes_no[0]) + '.' + columns[0][0] + '_' + columns[0][1]
    sql += ' = Q' + str(query_no) + '_N' + str(nodes_no[1]) + '.' + columns[1][0] + '_' + columns[1][1]
    sql += ';'
    return sql

def sql_drop_temp(query_no, children):
    for i in children:
        sql = 'drop table Q' + str(query_no) + '_N' + str(i) + ';'
        print(sql) #实际运行应当执行drop

server_conn = [None, None, None, None, None]
db_conn = [None, None, None, None, None]

def server_conn_init(self_site):
    global server_conn
    if self_site != 1:
        conn = grpc.insecure_channel('10.77.70.61:8883')
        server_conn[1] = net_pb2_grpc.NetServiceStub(channel=conn) #site1在server 61:8883
    if self_site != 2:
        conn = grpc.insecure_channel('10.77.70.61:8885')
        server_conn[2] = net_pb2_grpc.NetServiceStub(channel=conn) #site2在server 61:8885
    if self_site != 3:
        conn = grpc.insecure_channel('10.77.70.62:8883')
        server_conn[3] = net_pb2_grpc.NetServiceStub(channel=conn) #site3在server 62:8883
    if self_site != 4:
        conn = grpc.insecure_channel('10.77.70.63:8883')
        server_conn[4] = net_pb2_grpc.NetServiceStub(channel=conn) #site4在server 63:8883

def db_conn_init(self_site):
    global db_conn
    _MYSQL_USER = "root"
    _MYSQL_PASSWORD = "Ddb123$%^"
    _DB = "db1"
    if self_site == 1:
        db_conn[1] = Conndb(user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB) #site1
    if self_site == 2:
        _DB = "db2"
        db_conn[2] = Conndb(user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB) #site2
    elif self_site == 3:
        db_conn[3] = Conndb(user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB) #site3
    elif self_site == 4:
        db_conn[4] = Conndb(user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB) #site4

def dfs_execute(query_no, node_no, str_nodes, self_site): #return node_no,columns,data,trans_vol data=tuple(tuple)
    global server_conn
    nodes = str2nodes(str_nodes)
    #print(query_no, node_no)
    if nodes[node_no].site != self_site: #不同站，grpc远程过程调用
        print('[grpc from Site' + str(self_site) + ' to Site' + str(nodes[node_no].site), end=']')
    
    print('Start Node' + str(node_no)) #同站
    
    dfs_node_no, str_columns, str_values, trans_vol = dfs(query_no, node_no, str_nodes)
    columns = eval(str_columns)
    values = eval(str_values)

    i = nodes[node_no]
    print(sql_create(query_no, i.id, columns)) #创建中间结果表
    if values != (): #若结果非空，把中间结果写入中间结果表
        print(sql_insert(query_no, i.id, columns, values))

    print('Finish Node' + str(node_no))
    
    if nodes[node_no].site != self_site: #不同站，累计传输量
        trans_vol += sys.getsizeof(str(values))
    
    return node_no, columns, values, trans_vol

def dfs(query_no, node_no, str_nodes): #return node_no,columns,data,trans_vol data=tuple(tuple)
    #print(node_no)
    global db_conn
    nodes = str2nodes(str_nodes)
    #print('dfs' + str(node_no) + ' on Site' + str(nodes[node_no].site))
    #if nodes[node_no].children != []:
    #    print('dfs' + str(node_no) + ' wait for ' + str(len(nodes[node_no].children)) + ' children')
    future_results = []
    if nodes[node_no].children != []:
        tasks = []
        with ThreadPoolExecutor(max_workers=5) as t:
            for i in nodes[node_no].children:
                task = t.submit(dfs_execute, query_no, i, str_nodes, nodes[node_no].site)
                tasks.append(task)
            for future in as_completed(tasks):
                future_results.append(future.result())
    
    i = nodes[node_no]
    columns = []
    values = ()
    values = ((300001, 'Xiaoming', 1), (300002,'Xiaohong', 1)) #实际运行没有这一行
    if i.type == 'fragment':
        columns = fragment_columns(i.tables[0], i.site)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(i.tables[0], columns[j])
        print('Node' + str(node_no), end=': ')
        print(sql_select_fragment(columns)) #实际运行时，调用db_conn[i.site]
        values = ((300001, 'Xiaoming', 1),) #实际运行时values是sql结果，没有这一行
    elif i.type == 'projection':
        tables, columns = table_column_from_pj(i.projection)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        print(sql_project(query_no, i.children[0], columns)) #实际运行应有values为sql结果
    elif i.type == 'join':
        tables, columns = table_column_from_pj(i.join)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        print(sql_join(query_no, i.children, columns)) #实际运行应有values为sql结果

        if [future_results[0][0], future_results[1][0]] != i.children:
            swap_t = future_results[0]
            future_results[0] = future_results[1]
            future_results[1] = swap_t
        columns = future_results[0][1] + future_results[1][1]
    elif i.type == 'select':
        tables, columns = table_column_from_pj([i.select_condi[0]])
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        print(sql_select(query_no, i.children[0], columns, i.select_condi))

        columns = future_results[0][1]
    elif i.type == 'union':
        print('Node' + str(node_no), end=': ')
        print(sql_union(query_no, i.children)) #实际运行应有values为sql结果
        columns = future_results[0][1]

    trans_vol = 0
    for j in future_results:
        trans_vol += j[3]
    
    sql_drop_temp(query_no, i.children) #drop temp tables
    return node_no, str(columns), str(values), trans_vol

def dfs_sql(conndb, sql, write_flag):
    #print('conndb ' + str(conndb))
    _host = conndb.host
    _user = conndb.user
    _password = conndb.password
    _database = conndb.database
    _conndb = Conndb(_host, _user, _password, _database)
    #print('_conndb ' + str(_conndb))
    if sql == '':
        return ()
    if write_flag:
        _conndb.write(sql)
        return ()
    else:
        data = _conndb.read(sql)
        return data

client = {}
def grpc_test(th, site): #jr测试grpc远程过程调用
    site2ipd = [None, 
                '10.77.70.61:8883:db1',
                '10.77.70.61:8885:db2',
                '10.77.70.62:8883:db1',
                '10.77.70.63:8883:db1'
                ]
    
    global client
    print('Site' + str(site + 1) + ' ' + str(th))
    if th == 4:
        return str(site + 1)
    
    nex_site = (site + 1) % 4
    response = client[site2ipd[nex_site + 1]].grpc_test(net_pb2.ret_grpc_test(th=th, site=site))
    res = response.res
    print('Site' + str(site + 1) + ' return from Site' + res)
    
    return str(site + 1)

'''
values = ((300001, 'Xiaoming', 1), (300002,'Xiaohong', 1))
for i in nodes:
    print(str(i.id) + ' ' + i.type, end=': ')
    if i.type == 'fragment':
        columns = fragment_columns(i.tables[0], i.site)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(i.tables[0], columns[j])
        print(sql_select_fragment(columns))
        print(sql_create(0, i.id, columns))
        print(sql_insert(0, i.id, columns, values))
    if i.type == 'projection':
        tables, columns = table_column_from_pj(i.projection)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print(sql_project(0, i.children[0], columns))
        print(sql_create(0, i.id, columns))
        print(sql_insert(0, i.id, columns, values))
    elif i.type == 'join':
        tables, columns = table_column_from_pj(i.join)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print(sql_join(0, i.children, columns))
        print(sql_create(0, i.id, columns))
        print(sql_insert(0, i.id, columns, values))
    elif i.type == 'union':
        print(sql_union(0, i.children))
'''

'''
_MYSQL_USER = "root"
_MYSQL_PASSWORD = "Ddb123$%^"
_DB = "db1"
conndb = Conndb(user=_MYSQL_USER, password=_MYSQL_PASSWORD, database=_DB)
result = conndb.read('show tables;')
print(result)
print(len(result))
print(result == ())
'''

'''
node0 = nodes[0]
print(node0)
dict0 = Node2dict(node0)
print(dict0)
node0_x = dict2Node(dict0)
print(node0_x)
print(NodeEqual(node0_x, node0))
'''

'''
str_nodes = nodes2str(nodes)
print(str_nodes)
nodes_x = str2nodes(str_nodes)
print(nodes_x)
print(nodesEqual(nodes, nodes_x))
'''


'''
### ex1
root = 20
result = dfs_execute(7, root, nodes2str(nodes), nodes[root].site)
print(result)
###
'''

'''
self_site = 1
server_conn_init(self_site)
grpc_test(0, self_site - 1)
'''

'''
fileout2 = open('tree_ex2.txt', 'w', encoding='utf-8')
nodes2 = pickle.loads(etcd.get('/tree')[0])
nodes2 = eval(nodes2str(nodes2))
for i in nodes2:
    fileout2.write('{')
    for j in list(i.items()):
        fileout2.write(str(j[0]) + '=' + str(j[1]) + '\n')
    fileout2.write('}\n\n')
fileout2.close()
'''

'''
query_no = 0
node_no = 15
nodes2 = pickle.loads(etcd.get('/tree')[0])
i = nodes2[node_no]
tables, columns = table_column_from_pj([i.select_condi[0]])
for j in range(0, len(columns)):
    columns[j] = table_column_type(tables[j], columns[j])
print('Node' + str(node_no), end=': ')
print(sql_select(query_no, i.children[0], columns, i.select_condi))
'''

'''
### ex2
query_no = 8
nodes2 = pickle.loads(etcd.get('/tree')[0])
root_no = nodes2[-1].id
result = dfs_execute(query_no, root_no, nodes2str(nodes2), nodes2[root_no].site)
sql_drop_temp(query_no, [nodes2[-1].id])
print(result)
###
'''

'''
_IP = '10.77.70.61'
_PORT = '8885'
_DB = "db2"

sitenames = eval(etcd.get('sitenames')[0])
root_site = None
for i in list(sitenames.items()):
    if i[1] == ':'.join([_IP, _PORT, _DB]):
        root_site = int(i[0][-1])
        break
print(sitenames)
print(root_site)
'''

#'''
fileout2 = open('tree_q6.txt', 'w', encoding='utf-8')
nodes2 = pickle.loads(etcd.get('/tree')[0])
nodes2 = eval(nodes2str(nodes2))
for i in nodes2:
    fileout2.write('{')
    for j in list(i.items()):
        fileout2.write(str(j[0]) + '=' + str(j[1]) + '\n')
    fileout2.write('}\n\n')
fileout2.close()
#'''