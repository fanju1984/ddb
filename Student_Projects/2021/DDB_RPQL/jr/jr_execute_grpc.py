import sys
from typing import List
from concurrent.futures import ThreadPoolExecutor, as_completed
import grpc
import pickle
from time import time
from time import localtime
from time import asctime
#from jr.jr_execute import sql_drop_temp
#import structures
#from structures import Node

sys.path.append("../")
from net import net_pb2, net_pb2_grpc
from server.connect import Conndb
import etcd3
from etcd.etcd import Etcd_S
import client.structures
from client.structures import Node
from client.create_tree import create_a_tree

etcd = etcd3.client(host="10.77.70.61", port=2379)

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
    print('fragment_site: ' + site)
    return eval(etcd.get('/table/' + table + '/fragment/' + str(site))[0])['columns']

def sql_create(db_conn, query_no, node_no, columns):
    temp_table_name = 'Q' + str(query_no) + '_N' + str(node_no)
    sql = 'create table ' + temp_table_name + ' ('
    
    str_columns = []
    for i in columns:
        str_columns.append(i[0] + '_' + i[1] + ' ' + i[2])
    sql += ', '.join(str_columns)
    sql += ');'
    
    dfs_sql(db_conn, sql, True) #create table

def sql_createindex(db_conn, query_no, node_no, columns):
    temp_table_name = 'Q' + str(query_no) + '_N' + str(node_no)

    str_columns = []
    for i in columns:
        str_columns.append(i[0] + '_' + i[1])
    for i in str_columns:
        sql = 'create index ' + i + '_index on ' + temp_table_name + ' (' + i + ');' 
        dfs_sql(db_conn, sql, True)

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
    print('SQL: ' + sql)
    sql += columns[0][0] + '_' + columns[0][1]
    for i in range(1, len(columns)):
        sql += ', ' + columns[i][0] + '_' + columns[i][1]
    
    sql += ') values'
    sql += value_tuple(values[0])
    for i in range(1, len(values)):
        sql += ', ' + value_tuple(values[i])
    
    sql += ';'
    print('SQL: insert into Q' + str(query_no) + '_N' + str(node_no) + ' generated')
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

def sql_join(db_conn, query_no, nodes_no, columns):
    '''
    if columns[0][0] == columns[1][0] and columns[0][1] == columns[1][1]:
        sql = 'select ' + columns[0][0] + '_' + columns[0][1]
    else:
        sql = 'select *'
    '''
    sql_createindex(db_conn, query_no, nodes_no[0], [columns[0]]) #在左表的join相关列上建索引

    sql = 'select *'
    sql += ' from Q' + str(query_no) + '_N' + str(nodes_no[0])
    sql += ', Q' + str(query_no) + '_N' + str(nodes_no[1])
    sql += ' where Q' + str(query_no) + '_N' + str(nodes_no[0]) + '.' + columns[0][0] + '_' + columns[0][1]
    sql += ' = Q' + str(query_no) + '_N' + str(nodes_no[1]) + '.' + columns[1][0] + '_' + columns[1][1]
    sql += ';'
    return dfs_sql(db_conn, sql, False)

def sql_drop_temp(query_no, children, db_conn):
    for i in children:
        sql = 'drop table Q' + str(query_no) + '_N' + str(i) + ';'
        #print(sql) #实际运行应当执行drop
        dfs_sql(db_conn, sql, True) #drop临时表

def temp_GC(db_conn):
    tbs = dfs_sql(db_conn, 'show tables;', False)
    for i in tbs:
        if i[0][0] == 'Q' and i[0].find('_') >= 0:
            dfs_sql(db_conn, 'drop table ' + i[0] + ';', True)
            print("drop table " + i[0] + "success")
    print('temp GC!')

def get_site2ipd():
    site2ipd = [None, 
                '10.77.70.61:8883:db1',
                '10.77.70.61:8885:db2',
                '10.77.70.62:8883:db1',
                '10.77.70.63:8883:db1'
                ]

    #'''
    site2ipd = eval(etcd.get('sitenames')[0])
    #'''

    return site2ipd

def dfs_execute(query_no, node_no, str_nodes, self_site, db_conn, client): 
    site2ipd = get_site2ipd()
    
    nodes = str2nodes(str_nodes)
    #print(query_no, node_no)
    
    #response
    dfs_node_no = None
    str_columns = None
    str_values = None
    trans_vol = None
    print(nodes[node_no].site, self_site)
    if nodes[node_no].site != self_site: #不同站，grpc远程过程调用
        print('[grpc from Site' + str(self_site) + ' to Site' + str(nodes[node_no].site) + '] Start Node' + str(node_no))
        try:
            conn = client[site2ipd[nodes[node_no].site]]
            response = conn.grpc_dfs(net_pb2.para_grpc_dfs(query_no=query_no, node_no=node_no, str_nodes=str_nodes))
        except KeyError as e:
            print('WARNING! ' + str(e) + ' NOT found')
            temp_GC(db_conn)
            return -1, [], (), nodes[node_no].site
        else:
            dfs_node_no = response.dfs_node_no 
            str_columns = response.str_columns
            str_values = response.str_values
            trans_vol = response.trans_vol + sys.getsizeof(str_values) #不同站，累计传输量
            
            #site check
            if dfs_node_no == -1:
                print('WARNING! Site' + str(trans_vol) + ' NOT found')
                temp_GC(db_conn)
                return -1, [], (), trans_vol
    else:
        print('_THIS Site Start Node' + str(node_no)) #同站
        dfs_node_no, str_columns, str_values, trans_vol = dfs(query_no, node_no, str_nodes, db_conn, client)
        if dfs_node_no == -1:
            print('WARNING! Site' + str(trans_vol) + ' NOT found')
            temp_GC(db_conn)
            return -1, [], (), trans_vol
    
    columns = eval(str_columns)
    values = eval(str_values)
    i = nodes[node_no]
    #print(sql_create(query_no, i.id, columns)) #创建中间结果表
    sql_create(db_conn, query_no, i.id, columns) #创建中间结果表及索引加速
    if values != (): #若结果非空，把中间结果写入中间结果表
        #print(sql_insert(query_no, i.id, columns, values))
        dfs_sql(db_conn, sql_insert(query_no, i.id, columns, values), True)

    print('Finish Node' + str(node_no))
    return node_no, columns, values, trans_vol #return node_no,columns,data,trans_vol data=tuple(tuple)

def grpc_dfs(self, request, context): 
    #get paras from request
    query_no = request.query_no
    node_no = request.node_no
    str_nodes = request.str_nodes
    
    #call dfs local
    #把servermaster.py里的 conndb、clients给dfs
    global client
    dfs_node_no, str_columns, str_values, trans_vol = dfs(query_no, node_no, str_nodes, self.conndb, client)

    #return
    return net_pb2.ret_grpc_dfs(dfs_node_no=dfs_node_no, str_columns=str_columns, str_values=str_values, trans_vol=trans_vol)

def dfs(query_no, node_no, str_nodes, db_conn, client): #return node_no,columns,data,trans_vol data=tuple(tuple)
    #print(node_no)
    nodes = str2nodes(str_nodes)
    #print('dfs' + str(node_no) + ' on Site' + str(nodes[node_no].site))
    #if nodes[node_no].children != []:
    #    print('dfs' + str(node_no) + ' wait for ' + str(len(nodes[node_no].children)) + ' children')
    future_results = []
    if nodes[node_no].children != []:
        tasks = []
        with ThreadPoolExecutor(max_workers=5) as t:
            for i in nodes[node_no].children:
                task = t.submit(dfs_execute, query_no, i, str_nodes, nodes[node_no].site, db_conn, client)
                tasks.append(task)
            for future in as_completed(tasks):
                future_results.append(future.result())
    
    #site check
    print('site check:')
    for i in future_results:
        #print(i)
        if i[0] == -1:
            print('WARNING! Site' + str(i[-1]) + ' NOT found')
            temp_GC(db_conn)
            return i[0], '', '', i[-1]

    i = nodes[node_no]
    columns = []
    values = ()
    #values = ((300001, 'Xiaoming', 1), (300002,'Xiaohong', 1)) #实际运行没有这一行
    if i.type == 'fragment':
        columns = fragment_columns(i.tables[0], i.site)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(i.tables[0], columns[j])
        print('Node' + str(node_no), end=': ')
        #print(sql_select_fragment(columns)) #实际运行时，调用db_conn[i.site]
        values = dfs_sql(db_conn, sql_select_fragment(columns), False) #values = fragment sql结果
    elif i.type == 'projection':
        tables, columns = table_column_from_pj(i.projection)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        #print(sql_project(query_no, i.children[0], columns)) #实际运行应有values为sql结果
        values = dfs_sql(db_conn, sql_project(query_no, i.children[0], columns), False) #values = project sql结果
    elif i.type == 'join':
        tables, columns = table_column_from_pj(i.join)
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        #print(sql_join(query_no, i.children, columns)) #实际运行应有values为sql结果
        values = sql_join(db_conn, query_no, i.children, columns) #values = join sql结果

        if [future_results[0][0], future_results[1][0]] != i.children:
            swap_t = future_results[0]
            future_results[0] = future_results[1]
            future_results[1] = swap_t
        
        _columns = []
        _value_k = []
        _value_k_th = 0
        _values = []
        for j in future_results:
            for k in j[1]:
                if k not in _columns:
                    _columns.append(k)
                    _value_k.append(_value_k_th)
                _value_k_th += 1
        for j in values:
            _value = []
            for k in _value_k:
                _value.append(j[k])
            _values.append(tuple(_value))
        columns = _columns
        values = tuple(_values)
        print(columns, values[0]) #join columns data

    elif i.type == 'select':
        tables, columns = table_column_from_pj([i.select_condi[0]])
        for j in range(0, len(columns)):
            columns[j] = table_column_type(tables[j], columns[j])
        print('Node' + str(node_no), end=': ')
        #print(sql_select(query_no, i.children[0], columns, i.select_condi)) #实际运行应有values为sql结果
        values = dfs_sql(db_conn, sql_select(query_no, i.children[0], columns, i.select_condi), False) #values = select sql结果

        columns = future_results[0][1]
    elif i.type == 'union':
        print('Node' + str(node_no), end=': ')
        #print(sql_union(query_no, i.children)) #实际运行应有values为sql结果
        values = dfs_sql(db_conn, sql_union(query_no, i.children), False) #values = union sql结果
        columns = future_results[0][1]

    trans_vol = 0
    for j in future_results:
        trans_vol += j[3]
    
    sql_drop_temp(query_no, i.children, db_conn) #drop temp tables
    return node_no, str(columns), str(values), trans_vol

'''
def dfs_sql(conndb, sql, write_flag):
    if sql == '':
        return ()
    if write_flag:
        conndb.write(sql)
        return ()
    else:
        print(sql)
        data = conndb.read(sql)
        return data
'''

def dfs_sql(conndb, sql, write_flag): #multi_thread conndb
    #site2ipd = get_site2ipd()
    #print('conndb ' + str(conndb))
    _host = conndb.host
    _user = conndb.user
    _password = conndb.password
    _database = conndb.database
    _conndb = Conndb(_host, _user, _password, _database)
    #print('_conndb ' + str(_conndb))
    print()
    if sql == '':
        return ()
    if write_flag:
        if sql.find('insert') < 0:
            print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql)
        else:
            values_loc = sql.find('values')
            print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql[0 : values_loc])
        _conndb.write(sql)
        if sql.find('insert') < 0:
            print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql + ' Finished')
        else:
            values_loc = sql.find('values')
            print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql[0 : values_loc] + ' Finished')
        return ()
    else:
        print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql)
        data = _conndb.read(sql)
        print(str(asctime(localtime(time()))) + ': ' + 'Mysql: ' + sql + ' Finished')
        return data

#for servermaster
def jr_execute(query_no, nodes, root_site, db_conn, client): #query_no is an int which means K_th query, nodes is the tree, root_site is _THIS, para_client is client{}, db_conn local, client local
    root_no = nodes[-1].id #the root of the tree should be the last node of nodes
    print(root_site)
    result = dfs_execute(query_no, root_no, nodes2str(nodes), root_site, db_conn, client)
    #print('Should be fucking end!')
    if result[0] == -1:
        print('WARNING! Site' + str(result[-1]) + ' NOT found')
        temp_GC(db_conn)
        return [], (), result[3]
    
    result_columns = []
    for i in result[1]:
        result_columns.append(i[0] + '.' + i[1])
    
    sql_drop_temp(query_no, [nodes[-1].id], db_conn) #drop temp tables of root
    #print('Should be fucking end!')
    return result_columns, result[2], result[3] #columns,data,trans_vol data=tuple(tuple)

def update_sitename(oldname, newname):
    #sitenames
    path = 'sitenames'
    obj = etcd.get(path)[0]
    if obj != None:
        context = eval(obj)
        context = list(context.items())
        for i in range(0, len(context)):
            if context[i][0] == oldname:
                context[i] = (newname, context[i][1])
        new_context = {}
        for i in context:
            new_context[i[0]] = i[1]
        #print(path, new_context)
        etcd.put(path, str(new_context))
    
    #table info
    path = '/table'
    obj = etcd.get(path)[0]
    if obj == None:
        return
    tables = eval(obj)
    #/site
    for i in tables:
        path = '/site/' + oldname + '/fragment/' + i
        obj = etcd.get(path)[0]
        if obj != None:
            etcd.delete(path)
            context = eval(obj)
            context['sitename'] = newname
            path = '/site/' + newname + '/fragment/' + i
            #print(path, context)
            etcd.put(path, str(context))
    #/table/table_name/fragment
    for i in tables:
        path = '/table/' + i + '/fragment/' + oldname
        obj = etcd.get(path)[0]
        if obj != None:
            etcd.delete(path)
            context = eval(obj)
            context['sitename'] = newname
            path = '/table/' + i + '/fragment/' + newname
            #print(path, context)
            etcd.put(path, str(context))
    #/table/table_name/lenfragment
    for i in tables:
        path = '/table/' + i + '/lenfragment/' + oldname
        obj = etcd.get(path)[0]
        if obj != None:
            etcd.delete(path)
            context = eval(obj)
            path = '/table/' + i + '/lenfragment/' + newname
            #print(path, context)
            etcd.put(path, str(context))

'''
from jr_execute_grpc import jr_execute
nodes = pickle.loads(etcd.get('/tree')[0])
sitenames = eval(etcd.get('sitenames')[0])
root_site = None
for i in list(sitenames.items()):
    if i[1] == ':'.join([_IP, _PORT, _DB]):
        root_site = int(i[0][-1])
        break
global client
jr_execute(0, nodes, root_site, self.conndb, client)
'''

#'''
#update_sitename('england', 'britain')
#update_sitename('britain', 'england')
#'''