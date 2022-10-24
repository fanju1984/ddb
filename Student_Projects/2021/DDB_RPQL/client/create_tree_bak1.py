import enum
from typing import final
from google.protobuf import message
import grpc
import sys
import re
import time
import etcd3
import pickle
import asyncio
from grpc import aio
from concurrent.futures import ThreadPoolExecutor, as_completed
from graphviz import Digraph

sys.path.append("../")
from meta import *
from utils import *
from net import net_pb2, net_pb2_grpc
from etcd.etcd import Etcd_S

from os import execv, stat
from structures import Attribute, Selection, Join, Fragment, Node, Table
import sqlparse
import copy
from graphviz import Digraph
import math
import numpy as np
import re
import json
import pickle

etcd = Etcd_S(host=getip(), port=2379)


def parsesql(query):
    result = sqlparse.parse(query)
    selects = []
    all_need_attributes = []
    final_attributes = []
    joins = []
    tables = []
    join_extra_selects = []
    afterfrom = 0
    # print('tttttttttttttttttttttttttttttttttttttttttttokens')
    # for token in result[0].tokens:
    #    print(token.value)
    if_project_all = 0
    for token in result[0].tokens:
        if token.value == "from":
            afterfrom = 1
        if token.value == "*":
            if_project_all = 1
        # print(type(token),token.ttype,token.value)
        if isinstance(token, sqlparse.sql.Identifier):
            # print(token.value, token.get_real_name(),token.get_parent_name())
            if token.get_parent_name():
                if token.get_parent_name() not in tables:
                    tables.append(token.get_parent_name())
                all_need_attributes.append(token.value)
            elif afterfrom == 0:
                all_need_attributes.append(token.value)
            else:
                if token.value not in tables:
                    tables.append(token.value)
                for i in range(len(all_need_attributes)):
                    if "." not in all_need_attributes[i]:
                        all_need_attributes[i] = (
                            token.value + "." + all_need_attributes[i]
                        )

        if isinstance(token, sqlparse.sql.IdentifierList):
            for iden in token.get_identifiers():
                if iden.get_parent_name():
                    if iden.get_parent_name() not in tables:
                        tables.append(iden.get_parent_name())
                    all_need_attributes.append(iden.value)
                elif afterfrom == 0:
                    all_need_attributes.append(iden.value)
                else:
                    if iden.value not in tables:
                        tables.append(iden.value)
                    for i in range(len(all_need_attributes)):
                        if "." not in all_need_attributes[i]:
                            all_need_attributes[i] = (
                                iden.value + "." + all_need_attributes[i]
                            )
            # print(token.value, token.get_real_name(),token.get_parent_name())
        final_attributes = copy.deepcopy(all_need_attributes)
        exclude_select_attributes = copy.deepcopy(all_need_attributes)
        if isinstance(token, sqlparse.sql.Where):
            for i in token.tokens:
                # print(type(i), i.ttype, i.value)
                if isinstance(i, sqlparse.sql.Comparison):
                    select = [0 for _ in range(3)]
                    afterequal = 0
                    sel_or_join = 0
                    for selectitem in i.tokens:
                        # print(type(selectitem),selectitem.ttype,selectitem.value)
                        if isinstance(selectitem, sqlparse.sql.Identifier):
                            if selectitem.get_parent_name() is None:
                                # all_need_attributes.append('.'.join([tables[0],selectitem.value]))
                                if afterequal == 0:
                                    select[0] = ".".join([tables[0], selectitem.value])
                                else:
                                    select[2] = ".".join([tables[0], selectitem.value])
                                    sel_or_join = 1
                            else:
                                # all_need_attributes.append(selectitem.value)
                                if afterequal == 0:
                                    select[0] = selectitem.value
                                else:
                                    select[2] = selectitem.value
                                    sel_or_join = 1
                        elif isinstance(
                            selectitem, sqlparse.sql.Token
                        ) and selectitem.value in [">", "=", "<"]:
                            select[1] = selectitem.value
                            afterequal = 1
                        elif (
                            isinstance(selectitem, sqlparse.sql.Token)
                            and selectitem.value.isdigit()
                        ):
                            select[2] = selectitem.value
                        elif (
                            isinstance(selectitem, sqlparse.sql.Token)
                            and selectitem.value
                        ):
                            select[2] = selectitem.value
                    if sel_or_join == 0:
                        selects.append(select)
                        all_need_attributes.append(select[0])
                    else:
                        joins.append(select)
                        all_need_attributes.append(select[0])
                        all_need_attributes.append(select[2])
                        exclude_select_attributes.append(select[0])
                        exclude_select_attributes.append(select[2])
    all_need_attributes = list(set(all_need_attributes))
    # tables = list(set(tables))

    for j in joins:
        select_attrs = [s[0] for s in selects]
        # print(j[0])
        if j[0] in select_attrs and j[2] not in select_attrs:
            oneselect = copy.deepcopy(selects[select_attrs.index(j[0])])
            oneselect[0] = j[2]
            join_extra_selects.append(oneselect)
        if j[1] in select_attrs and j[2] not in select_attrs:
            oneselect = copy.deepcopy(selects[select_attrs.index(j[1])])
            oneselect[0] = j[0]
            join_extra_selects.append(oneselect)

    # print('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')
    """
    print('selects',selects)
    print('joins',joins)
    print('all_need_attributes',all_need_attributes)
    print('tables',tables)
    print('join_extra_selects',join_extra_selects)
    print('final_attributes',final_attributes)
    """
    return (
        if_project_all,
        selects,
        joins,
        all_need_attributes,
        tables,
        final_attributes,
        exclude_select_attributes,
        join_extra_selects,
    )


def operator_conflict(oa, ob, va, vb):
    if oa == ">" and ob in ["<", "<=", "="]:
        if va >= vb:
            return False
    if oa == "<" and ob in [">", ">=", "="]:
        if va <= vb:
            return False
    if oa == ">=" and ob in ["<"]:
        if va >= vb:
            return False
    if oa == ">=" and ob in ["<=", "="]:
        if va > vb:
            return False
    if oa == "<=" and ob in [">"]:
        if va <= vb:
            return False
    if oa == "<=" and ob in [">=", "="]:
        if va < vb:
            return False
    if oa == "=" and ob in [">"]:
        if va <= vb:
            return False
    if oa == "=" and ob in ["<"]:
        if va >= vb:
            return False
    if oa == "=" and ob in [">="]:
        if va < vb:
            return False
    if oa == "=" and ob in ["<="]:
        if va > vb:
            return False
    if oa == "=" and ob == "=":
        if va != vb:
            return False
    return True


# def join_pair(joins,index,alreadyin)


def create_a_tree(sql):
    statistics = {}

    for attr in etcd.get_prefix("/attrinfo"):
        # print(attr)
        tableinfo = eval(attr[0])
        # print(tableinfo)
        if tableinfo["type"] in ["U", "N"]:
            statistics[tableinfo["attr"]] = [tableinfo["type"]] + [
                int(a) for a in tableinfo["numbers"]
            ]
        elif tableinfo["type"] == "P":
            proportions = {}
            lastkey = None
            for i in range(len(tableinfo["numbers"])):
                if i % 2:
                    proportions[lastkey] = float(tableinfo["numbers"][i])
                else:
                    if tableinfo["numbers"][i].isdigit():
                        proportions[int(tableinfo["numbers"][i])] = None
                        lastkey = int(tableinfo["numbers"][i])
                    else:
                        proportions[tableinfo["numbers"][i].strip("'")] = None
                        lastkey = tableinfo["numbers"][i].strip("'")
            statistics[tableinfo["attr"]] = [tableinfo["type"], proportions]
    """
    #print(statistics)
    site2ip = {}
    #print('aaaaaaaaaaaaaaa')
    sitenames = etcd.get("sitenames")
    #print([s for s in sitenames])
    sitenames = eval(sitenames[0])
    for s in sitenames.keys():
        site2ip[int(s[-1:])] = sitenames[s]
    """
    # print(site2ip)
    def copy_tree(root, parentid):
        # print('copy',root.id,root.children)
        if root.children == []:
            n3 = copy.deepcopy(root)  # 还得复制孩子
            n3.id = nodes[len(nodes) - 1].id + 1
            n3.parent = parentid
            nodes.append(n3)
            # print('copy的id',n3.id)
            return n3
        else:
            n3 = copy.deepcopy(root)  # 还得复制孩子
            n3.id = nodes[len(nodes) - 1].id + 1
            n3.parent = parentid
            nodes.append(n3)
            # print('中间节点',n3.id)
            """
            n3.children = [nodes[len(nodes)-1].id+1]
            copy_tree(nodes[root.children[0]],root.id)
            if len(root.children)>1:
                n3.children.append(nodes[len(nodes)-1].id+1)
                copy_tree(nodes[root.children[1]],root.id)
            """
            # print(n3.children)
            for i in range(len(n3.children)):
                # print('inrecurse',n3.f_id,n3.children[i],n3.id)
                copy_tree(nodes[n3.children[i]], n3.id)
            # print(n3.id)
            return n3

    def count_pro_size(origin_size, origin_attrs, new_attrs):
        origin_sum = 0
        new_sum = 0
        for a in origin_attrs:
            origin_sum += attr_sta[a]
        for a in new_attrs:
            new_sum += attr_sta[a]
        return origin_size * (new_sum / origin_sum)

    def string_a(a):
        for t in a:
            new_t = [str(x) for x in t]
            s_a.append(" ".join(new_t))
        return s_a

    def add_num(cand, cur):
        num_cand = cand.split(" ")
        num_cur = cur.split(" ")
        d_cnt = 1
        for i, num in enumerate(num_cur):
            if num not in num_cand[i]:
                if "|" not in num_cand[i]:
                    num_cand[i] = "(" + num_cand[i] + "|" + num + ")"
                else:
                    num_cand[i] = num_cand[i][:-1] + "|" + num + ")"
            if "|" in num_cand[i]:
                d_cnt = d_cnt * len(num_cand[i].split("|"))
        new_cand = " ".join(num_cand)
        return new_cand, d_cnt

    def check(cand, cur):
        new_cand, d_cnt = add_num(cand, cur)
        proper_one = []
        for ss in s_a:
            if re.match(new_cand, ss):
                proper_one.append(ss)
        if ans[cand] + len(proper_one) + 1 == d_cnt:
            for ss in proper_one:
                s_a.remove(ss)
            ans[new_cand] = d_cnt
            ans.pop(cand)
            return True
        return False

    def count_size(origin_size, origin_range=None, select_condition=None):
        info = statistics[select_condition[0]]
        # print("info!!!!!!!!!!!!!!!!!!!!!!!",info)
        # print(origin_size,select_condition)
        chonghe = 0
        if select_condition[2].isdigit():
            selectnum = int(select_condition[2])
        else:
            selectnum = select_condition[2]
        if origin_range:
            for o in origin_range:
                # print('.'.join([o.attr.table,o.attr.attr]),select_condition[0])
                if ".".join([o.attr.table, o.attr.attr]) == select_condition[0]:
                    chonghe = 1
                    # print('select_condition',select_condition)
                    if info[0] == "U":
                        if o.operator in ["<", "<="] and select_condition[1] in [
                            "<",
                            "<=",
                        ]:
                            return (
                                origin_size
                                * (o.num_value - info[1])
                                / (selectnum - info[1])
                            )
                        if o.operator in [">", ">="] and select_condition[1] in [
                            "<",
                            "<=",
                        ]:
                            return (
                                origin_size
                                * (selectnum - o.num_value)
                                / (info[2] - o.num_value)
                            )
                        if o.operator in ["<", "<="] and select_condition[1] in [
                            ">",
                            ">=",
                        ]:
                            return (
                                origin_size
                                * (o.num_value - selectnum)
                                / (o.num_value - info[1])
                            )
                        if o.operator in [">", ">="] and select_condition[1] in [
                            ">",
                            ">=",
                        ]:
                            # print('finddddddddddddddddd!!!!!!!!!!!!!!!!!!!!!!')
                            # print(origin_size,info[2],o.num_value,selectnum)
                            return (
                                origin_size
                                * (info[2] - selectnum)
                                / (info[2] - o.num_value)
                            )
        if chonghe == 0 or origin_range == None:
            if info[0] == "P":
                if type(selectnum) == type(1):
                    return origin_size * info[1][selectnum]
                else:
                    return origin_size * info[1][selectnum].strip("'")
            if info[0] == "U":
                if select_condition[1] in [">=", ">"]:
                    return origin_size * (info[2] - selectnum) / (info[2] - info[1])
                elif select_condition[1] in ["<=", "<"]:
                    return origin_size * (selectnum - info[1]) / (info[2] - info[1])
            if info[0] == "N":
                # sumpro = 0
                # print("进来了！！！！！")
                seppro = [0 for _ in range(info[3], info[4] + 1)]
                for i in range(info[3], info[4] + 1):
                    seppro[i - info[3]] = (
                        1
                        / math.sqrt(2 * 3.14 * info[2])
                        * math.exp(-(i - info[1]) * (i - info[1]) / (2 * info[2]))
                    )
                seppro = list(np.array(seppro) / np.sum(seppro))

                # print(seppro)
                if select_condition[1] == ">":
                    return origin_size * (
                        sum(seppro[selectnum + 1 - info[3] : info[4] + 1 - info[3]])
                    )
                if select_condition[1] == ">=":
                    return origin_size * (
                        sum(seppro[selectnum - info[3] : info[4] + 1 - info[3]])
                    )
                if select_condition[1] == "<":
                    # print("进来了！！！")
                    # print(sum(seppro[info[3]-info[3]:selectnum-info[3]]))
                    return origin_size * (
                        sum(seppro[info[3] - info[3] : selectnum - info[3]])
                    )
                if select_condition[1] == "<=":
                    return origin_size * (
                        sum(seppro[info[3] - info[3] : selectnum + 1 - info[3]])
                    )

    selectionlist = []
    attributelist = []
    fragmentlist = []
    table_map = {}
    nodes = []
    f_id = 0
    tabless = eval(etcd.get("/table")[0])
    # tabless = ['Publisher','Book','Customer','Orders']
    # print('tabless',tabless)
    fragnum = 0
    f_id_table_id = []
    f_id_s_id = []
    maxsite_ = 0
    sid2sname = {}
    sname2sid = {}
    for t in range(len(tabless)):
        site = 0
        # print(list(etcd.get_prefix("/table/" + tabless[t] + "/fragment")))
        # print(etcd.get("/table/" + tabless[t] + "/fragment/1")[1].key)
        for i in etcd.get_prefix("/table/" + tabless[t] + "/fragment"):
            fragnum += 1
            sname = (
                i[1]
                .key.decode("utf-8")
                .replace("/table/" + tabless[t] + "/fragment/", "")
            )
            if (site + 1) not in sid2sname:
                # print(sname)
                sid2sname[site + 1] = sname
                sname2sid[sname] = site + 1
            f_id_table_id.append(t)
            f_id_s_id.append(sname2sid[sname] - 1)
            site += 1
        if site > maxsite_:
            maxsite_ = site
    # print('aaaaaaaaaaaaaaaaaaaaaaaaaaaa')
    # print(sid2sname)
    # if_frag_select = [0 for _ in range(fragnum)]
    attr_sta = {}
    for t in range(len(tabless)):
        for i in etcd.get_prefix("/table/" + tabless[t] + "/fragment"):
            finfo = eval(i[0])
        # print('finfo',finfo['columns'])
        # print("/table/" + tabless[t] + "/columns")
        cinfo = etcd.get("/table/" + tabless[t] + "/columns")[0]
        # print(cinfo)
        cinfo = eval(cinfo)
        cinfo = [c[0] for c in cinfo]
        for c in cinfo:
            # print("/table/" + tabless[t] + "/columnsize/" + str(c))
            try:
                columnsize = eval(
                    etcd.get("/table/" + tabless[t] + "/columnsize/" + str(c))[0]
                )
            except:
                columnsize = 0
            # print(columnsize)
            attr_sta[tabless[t] + "." + str(c)] = int(columnsize)
    # attr_sta['Customer.name'] = 9
    # print(attr_sta)

    # f_id_table_id = [0,0,0,0,1,1,1,2,2,3,3,3,3]
    # f_id_s_id = [0,1,2,3,0,1,2,0,1,0,1,2,3]
    # print('maxsite',maxsite_)
    sitenums = [[0 for _ in range(maxsite_)] for _ in range(maxsite_)]  # 待改
    dot = Digraph(comment="The Round Table", format="svg")
    (
        if_project_all,
        selects,
        joins,
        all_need_attributes,
        tables,
        final_attributes,
        exclude_select_attribute,
        join_extra_selects,
    ) = parsesql(sql)

    f_id = -1
    for t in range(len(tabless)):
        site = 0
        s_id = 0
        frlist = []
        for i in etcd.get_prefix("/table/" + tabless[t] + "/fragment"):
            sname = (
                i[1]
                .key.decode("utf-8")
                .replace("/table/" + tabless[t] + "/fragment/", "")
            )
            s_id += 1
            f_id += 1
            # print("sss")
            finfo = eval(i[0])
            attrl = []
            for a in finfo["columns"]:
                if tabless[t] == "Customer" and a == "id":
                    attrl.append(
                        Attribute("Customer", "id", f_id=f_id, if_join_attr=True)
                    )
                else:
                    attrl.append(Attribute(tabless[t], a))
            attributelist.extend(attrl)
            hconds = []
            vconds = []
            if finfo["conditions"] != "":
                conditionlist = finfo["conditions"].split("and")
                # print('conditionlist',conditionlist)
                for comp in conditionlist:
                    # print(comp)
                    tempcomp = True
                    # print('comp',comp)
                    left, op, right = re.findall("(.+?)\s*([>=<]+)\s*(.+)", comp)[0]
                    left = left.strip(" ")
                    right = right.strip(" '\"")
                    right = right.strip("'")
                    op = op.strip(" ")
                    # print(left, op, right)
                    compattr = None
                    for at in attrl:
                        if at.attr == left:
                            compattr = at
                    if right.isdigit():
                        # print('Selection',compattr.table,compattr.attr,op,f_id,s_id,int(right))
                        hconds.append(
                            Selection(
                                compattr,
                                op,
                                f_id,
                                sname2sid[sname],
                                num_value=int(right),
                            )
                        )
                    else:
                        # print('Selection',compattr.table,compattr.attr,op,f_id,s_id,right)
                        hconds.append(
                            Selection(
                                compattr, op, f_id, sname2sid[sname], str_value=right
                            )
                        )
            else:
                for at in attrl:
                    if at.attr in finfo["columns"]:
                        vconds.append(at)
            if finfo["conditions"] != "":
                try:
                    lenrow = int(
                        etcd.get("/table/" + tabless[t] + "/lenfragment/" + sname)[0]
                    )
                except:
                    lenrow = 0
                rowsize = 0
                cinfo = eval(etcd.get("/table/" + tabless[t] + "/columns")[0])
                cinfo = [c[0] for c in cinfo]
                for c in cinfo:
                    # print("/table/" + tabless[t] + "/columnsize/"+str(c))
                    try:
                        rowsize += int(
                            eval(
                                etcd.get(
                                    "/table/" + tabless[t] + "/columnsize/" + str(c)
                                )[0]
                            )
                        )
                    except:
                        rowsize += 0
                frlist.append(
                    Fragment(
                        f_id,
                        tabless[t],
                        sname2sid[sname],
                        lenrow * rowsize,
                        0,
                        hori_condi=hconds,
                    )
                )
                selectionlist.extend(hconds)
            else:
                try:
                    lenrow = int(
                        etcd.get("/table/" + tabless[t] + "/lenfragment/" + sname)[0]
                    )
                except:
                    lenrow = 0
                # lenrow = int(etcd.get("/table/" + tabless[t] + "/lenfragment/"+str(sname2sid[sname]))[0])
                rowsize = 0
                # cinfo = eval(etcd.get("/table/" + tabless[t] + "/columns")[0])
                # cinfo = [c[0] for c in cinfo]
                for c in finfo["columns"]:
                    # print("/table/" + tabless[t] + "/columnsize/"+str(c))
                    try:
                        rowsize += int(
                            eval(
                                etcd.get(
                                    "/table/" + tabless[t] + "/columnsize/" + str(c)
                                )[0]
                            )
                        )
                    except:
                        rowsize += 0
                frlist.append(
                    Fragment(
                        f_id,
                        tabless[t],
                        sname2sid[sname],
                        lenrow * rowsize,
                        1,
                        verti_condi=vconds,
                    )
                )
        fragmentlist.extend(frlist)
        cinfo = eval(etcd.get("/table/" + tabless[t] + "/columns")[0])
        cinfo = [tabless[t] + "." + c[0] for c in cinfo]
        table_map[tabless[t]] = Table(tabless[t], frlist, cinfo)
    if if_project_all == 1:
        for ta in tables:
            final_attributes.extend(table_map[ta].attributes)
        final_attributes = list(set(final_attributes))
        all_need_attributes = copy.deepcopy(final_attributes)
        exclude_select_attribute = copy.deepcopy(final_attributes)

    """
    for f in t_pub.frag_list:                                #表的所有分片
        if f.hor_or_ver==0:
            for scon in f.hori_condi:
                '.'.join([scon.attr.table,scon.attr.attr])       #所有水平分片涉及到的属性
    """
    select_attr = [".".join([se.attr.table, se.attr.attr]) for se in selectionlist]
    for t in tables:
        for frag in table_map[t].frag_list:
            # print('frag',frag.id)
            if frag.hor_or_ver == 0:
                n = Node(
                    len(nodes),
                    "fragment",
                    -1,
                    [],
                    [t],
                    frag.s_id,
                    frag.size,
                    columns=table_map[t].attributes,  # 列
                    f_id=frag.id,
                    f_name=frag.table,
                    top=len(nodes),
                )
                nodes.append(n)
            else:
                n = Node(
                    len(nodes),
                    "fragment",
                    -1,
                    [],
                    [t],
                    frag.s_id,
                    frag.size,
                    columns=[
                        ".".join([at.table, at.attr]) for at in frag.verti_condi
                    ],  # 列
                    f_id=frag.id,
                    f_name=frag.table,
                    top=len(nodes),
                )
                nodes.append(n)
            sitenums[n.site - 1][f_id_table_id[n.f_id]] = n.size

    # print()
    if len(selects + join_extra_selects) > 0:
        for s in selects + join_extra_selects:  # query中涉及到的属性
            if s[0] in select_attr:  # 如果涉及的属性在分片属性里
                # print(s[0])
                frs = [
                    i for i in range(len(select_attr)) if select_attr[i] == s[0]
                ]  # 找到那个属性
                # print(frs)
                for j in frs:
                    if selectionlist[j].num_value:
                        if not operator_conflict(
                            s[1],
                            selectionlist[j].operator,
                            float(s[2]),
                            selectionlist[j].num_value,
                        ):  # 如果不冲突
                            # print(s)
                            # print(selectionlist[j].operator,selectionlist[j].num_value)#去除
                            # n = Node(len(nodes),'fragment',-1,-1,['Publisher'],selectionlist[j].s_id,0,
                            #        fragment_id=selectionlist[j].f_id)
                            for n in nodes:
                                if n.f_id == selectionlist[j].f_id:
                                    n.if_valid = False
                                    sitenums[n.site - 1][f_id_table_id[n.f_id]] = 0
                                    break
                    if selectionlist[j].str_value:
                        # print("找到了！！！！！！！！！！！！！！！！")
                        if not operator_conflict(
                            s[1],
                            selectionlist[j].operator,
                            s[2].strip("'"),
                            selectionlist[j].str_value,
                        ):  # 如果不冲突
                            # print(s)
                            # print(selectionlist[j].operator,selectionlist[j].str_value) #去除
                            # n = Node(len(nodes),'fragment',-1,-1,['Publisher'],selectionlist[j].s_id,0,
                            #        fragment_id=selectionlist[j].f_id)
                            for n in nodes:
                                if n.f_id == selectionlist[j].f_id:
                                    # nodes.remove(n)
                                    sitenums[n.site - 1][f_id_table_id[n.f_id]] = 0
                                    n.if_valid = False
                                    break

    for n in nodes:  # 剪掉vertical或projection
        if fragmentlist[n.f_id].hor_or_ver == 1:
            # print(n.f_id)
            include_attrs = [
                ".".join([v.table, v.attr])
                for v in fragmentlist[n.f_id].verti_condi
                if v.if_join_attr == False
            ]
            # print(all_need_attributes,include_attrs)
            if len(set(all_need_attributes).intersection(set(include_attrs))) == 0:
                # nodes.remove(n)
                # print("remove啦！！！！！！！！！！！！！！！！！1")
                n.if_valid = False
                sitenums[n.site - 1][f_id_table_id[n.f_id]] = 0

    need_select_attrs = [s[0] for s in selects]  # select vertical并join
    special_attr = "Customer.rank"
    for n in nodes:
        if not n.if_valid:
            continue
        if n.f_id != None and fragmentlist[n.f_id].hor_or_ver == 1:
            for v in fragmentlist[n.f_id].verti_condi:
                if ".".join([v.table, v.attr]) in need_select_attrs:
                    index = need_select_attrs.index(".".join([v.table, v.attr]))
                    new_size = n.size
                    if selects[index][0] in statistics.keys():
                        new_size = int(
                            count_size(n.size, select_condition=selects[index])
                        )  # vertical按理来说本身不会有范围
                    new = Node(
                        nodes[len(nodes) - 1].id + 1,
                        "select",
                        parent=-1,
                        children=[n.id],
                        tables=n.tables,
                        site=n.site,
                        size=new_size,
                        columns=n.columns,
                        select_condi=selects[index],
                    )
                    sitenums[n.site - 1][f_id_table_id[n.f_id]] = new_size
                    n.parent = nodes[len(nodes) - 1].id + 1
                    n.top = nodes[len(nodes) - 1].id + 1
                    nodes.append(new)
                    if (
                        ".".join([v.table, v.attr]) == special_attr
                        and special_attr not in final_attributes
                    ):
                        new_size = int(
                            count_pro_size(new.size, n.columns, ["Customer.rank"])
                        )  # 如果rank不需要就剪掉
                        # print("这里project了嘛？？？？？")
                        new1 = Node(
                            nodes[len(nodes) - 1].id + 1,
                            "projection",
                            parent=-1,
                            children=[new.id],
                            tables=n.tables,
                            site=n.site,
                            size=new_size,
                            columns=["Customer.id"],
                            projection=["Customer.id"],
                        )
                        sitenums[n.site - 1][f_id_table_id[n.f_id]] = new_size
                        new.parent = nodes[len(nodes) - 1].id + 1
                        n.top = nodes[len(nodes) - 1].id + 1
                        nodes.append(new1)
        if n.f_id != None and fragmentlist[n.f_id].hor_or_ver == 0:  # horizontal的select
            # if n.f_id==0:
            #    print('aaaaaaaaaaaa')
            # print("找到了！！！！！！！！！！！！！11")
            need_select_tables = [n.split(".")[0] for n in need_select_attrs]
            # if n.f_id==0:
            #    print('table--------------------',fragmentlist[n.f_id].table)
            # print('need select table',need_select_tables)
            # print(fragmentlist[n.f_id].table)
            if fragmentlist[n.f_id].table in need_select_tables:
                # print(fragmentlist[n.f_id].table)
                index = need_select_tables.index(fragmentlist[n.f_id].table)
                origin_range = fragmentlist[n.f_id].hori_condi
                # print('aaa',selects[index][0],origin_range[0].attr.table+'.'+origin_range[0].attr.attr)
                # print('aaa',selects[index][1],origin_range[0].operator)
                # print('aaa',selects[index][2],origin_range[0].str_value)
                noneedselect = 0
                # print("进来了吗？")
                for i in range(len(origin_range)):
                    # print(selects[index])
                    if (
                        selects[index][0]
                        == origin_range[i].attr.table + "." + origin_range[i].attr.attr
                        and selects[index][1] == origin_range[i].operator
                        and (
                            selects[index][2].strip("'") == origin_range[i].str_value
                            or (
                                selects[index][2].isdigit()
                                and int(selects[index][2]) == origin_range[i].num_value
                            )
                        )
                    ):
                        noneedselect = 1
                if noneedselect == 0:
                    new_size = n.size
                    if selects[index][0] in statistics.keys():
                        new_size = int(
                            count_size(
                                n.size,
                                origin_range=fragmentlist[n.f_id].hori_condi,
                                select_condition=selects[index],
                            )
                        )
                    # print("create node呀！！！！！！！！！！！！！！！！")
                    new = Node(
                        nodes[len(nodes) - 1].id + 1,
                        "select",
                        parent=-1,
                        children=[n.id],
                        tables=n.tables,
                        site=n.site,
                        size=new_size,
                        columns=n.columns,
                        select_condi=selects[index],
                    )
                    sitenums[n.site - 1][f_id_table_id[n.f_id]] = new_size
                    n.parent = nodes[len(nodes) - 1].id + 1
                    n.top = nodes[len(nodes) - 1].id + 1
                    nodes.append(new)

            if fragmentlist[n.f_id].table in tables:
                all_this_need_attributes = [
                    at
                    for at in exclude_select_attribute
                    if at.startswith(fragmentlist[n.f_id].table)
                ]
                # print("all this need attributes",all_this_need_attributes)
                # print(exclude_select_attribute)
                # if n.f_id==0:
                #    print('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')
                # print(all_this_need_attributes,len(all_this_need_attributes),len(table_map[fragmentlist[n.f_id].table].attributes))
                if len(all_this_need_attributes) < len(
                    table_map[fragmentlist[n.f_id].table].attributes
                ):
                    # print(all_this_need_attributes,table_map[fragmentlist[n.f_id].table].attributes)
                    n1 = n
                    while n1.parent != -1:
                        for no in nodes:
                            if no.id == n1.parent:
                                n1 = no
                    # origin_attrs = [table_map[fragmentlist[n.f_id].table].table+'.'+a for a in table_map[fragmentlist[n.f_id].table].attributes]
                    origin_attrs = [
                        a for a in table_map[fragmentlist[n.f_id].table].attributes
                    ]
                    new_size = int(
                        count_pro_size(n1.size, origin_attrs, all_this_need_attributes)
                    )
                    sitenums[n.site - 1][f_id_table_id[n.f_id]] = new_size
                    new = Node(
                        nodes[len(nodes) - 1].id + 1,
                        "projection",
                        parent=-1,
                        children=[],
                        tables=n.tables,
                        site=n.site,
                        size=new_size,
                        columns=all_this_need_attributes,
                        projection=all_this_need_attributes,
                    )
                    """
                    n1 = n
                    while n1.parent!=-1:
                        for no in nodes:
                            if no.id==n1.parent:
                                n1 = no
                    """
                    new.children.append(n1.id)
                    n1.parent = nodes[len(nodes) - 1].id + 1
                    n.top = nodes[len(nodes) - 1].id + 1
                    # print("add projection n.top", n.id, nodes[len(nodes)-1].id+1)
                    nodes.append(new)
    # for f in fragmentlist:
    #    print(f.hor_or_ver)
    verticals = [
        n
        for n in nodes
        if n.type == "fragment"
        and fragmentlist[n.f_id].hor_or_ver == 1
        and n.if_valid == True
    ]
    # print('verticalslen',verticals)
    if len(verticals) > 1:
        joinattr = [
            ".".join([v.table, v.attr])
            for v in fragmentlist[verticals[0].f_id].verti_condi
            if v.if_join_attr == True
        ]
        # print(joinattr)
        new = Node(
            nodes[len(nodes) - 1].id + 1,
            "join",
            parent=-1,
            children=[],
            tables=verticals[0].tables,
            site=verticals[0].site,  # 待改
            size=0,
            columns=[],
            join=[joinattr[0], joinattr[0]],
        )
        minn = verticals[0]
        maxsite = -1
        maxsize = 0
        for n in verticals:
            n1 = n
            while n1.parent != -1:
                for no in nodes:
                    if no.id == n1.parent:
                        n1 = no
            n1.parent = nodes[len(nodes) - 1].id + 1
            if n1.size < minn.size:
                minn = n1
            if n1.size > maxsize:
                maxsize = n1.size
                maxsite = n1.site
            new.children.append(n1.id)
            new.columns.extend(n1.columns)
            n.top = new.id
        # print("minn.size是",minn.size)
        if minn.type == "projection":
            new.size = int(minn.size / 4 * (4 + 9))
        else:
            new.size = int(minn.size / (4 + 4) * (4 + 4 + 9))
        # new.size = 114000*
        new.columns = list(set(new.columns))
        sitenums[maxsite - 1][f_id_table_id[n.f_id]] = new.size
        for n in verticals:
            if n.site != maxsite:
                sitenums[n.site - 1][f_id_table_id[n.f_id]] = 0
        new.site = maxsite
        nodes.append(new)

    # join
    all_pairs = []
    conflict_pairs = []
    join_index = -1
    # print("为什么会有union？？？？？",len(joins))
    if len(joins) > 0:
        ad_joins = copy.deepcopy(joins)  # 把joins写成首尾相接的形式
        last_table = ad_joins[0][-1].split(".")[0]
        for a in range(1, len(ad_joins)):
            if ad_joins[a][0].split(".")[0] != last_table:
                tmp = ad_joins[a][0]
                ad_joins[a][0] = ad_joins[a][-1]
                ad_joins[a][-1] = tmp
            last_table = ad_joins[a][-1].split(".")[0]
        joins = copy.deepcopy(ad_joins)
    if len(joins) > 0:
        exist_fids = [n.f_id for n in nodes if n.f_id != None and n.if_valid == True]
        for j in joins:
            join_index += 1
            jiu_length = len(all_pairs)
            # print(j)
            join_nodes = []
            f1s = table_map[j[0].split(".")[0]].frag_list
            f2s = table_map[j[2].split(".")[0]].frag_list
            t1_vertical_already = 0
            t2_vertical_already = 0
            n1s = []
            n2s = []
            for f1 in f1s:
                # print(f1.id)
                if f1.id in exist_fids and f1.hor_or_ver == 0:
                    for f2 in f2s:
                        # print(f2.id)
                        if f2.id in exist_fids and f2.hor_or_ver == 0:
                            if_conflict = 0
                            for s1 in f1.hori_condi:
                                if s1.attr.attr == j[0].split(".")[1]:
                                    # print(s1.attr.attr,j[0].split('.')[1])
                                    for s2 in f2.hori_condi:
                                        if s2.attr.attr == j[2].split(".")[1]:
                                            # print(s2.attr.attr,j[2].split('.')[1])
                                            if s1.num_value and s2.num_value:
                                                # print(s1.operator,s2.operator,s1.num_value,s2.num_value)
                                                if not operator_conflict(
                                                    s1.operator,
                                                    s2.operator,
                                                    s1.num_value,
                                                    s2.num_value,
                                                ):
                                                    # print(f1.id,f2.id)
                                                    # print(s1.attr.attr,s1.operator,s1.num_value)
                                                    # print(s2.attr.attr,s2.operator,s2.num_value)
                                                    conflict_pairs.append(
                                                        [f1.id, f2.id]
                                                    )
                                                    if_conflict = 1
                                            if s1.str_value and s2.str_value:
                                                if not operator_conflict(
                                                    s1.operator,
                                                    s2.operator,
                                                    s1.str_value,
                                                    s2.str_value,
                                                ):
                                                    conflict_pairs.append(
                                                        [f1.id, f2.id]
                                                    )
                                                    if_conflict = 1
                            if if_conflict == 0:
                                # print('aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa')
                                # print(f1.id,f2.id)
                                # print(join_index)
                                if join_index == 0:
                                    all_pairs.append([f1.id, f2.id])
                                    # print(all_pairs)
                                else:
                                    # print(all_pairs)
                                    origin_length = range(jiu_length)
                                    for i in origin_length:
                                        if f1.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f2.id)
                                            else:
                                                newone = copy.deepcopy(all_pairs[i])
                                                # print('newone',newone)
                                                all_pairs.append(newone)
                                                all_pairs[-1][join_index + 1] = f2.id

                                        elif f2.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f1.id)
                                            else:
                                                newone = copy.deepcopy(all_pairs[i])
                                                # print('newone',newone)
                                                all_pairs.append(newone)
                                                all_pairs[-1][join_index + 1] = f1.id
                        elif f2.id in exist_fids:  # f2属于垂直分片
                            if t2_vertical_already == 0:
                                t2_vertical_already = 1
                                n1 = None
                                n2 = None
                                for n in nodes:
                                    if n.f_id == f1.id:
                                        n1 = n
                                        while (
                                            n1.parent != -1
                                            and nodes[n1.parent].type != "join"
                                        ):
                                            n1 = nodes[n1.parent]
                                    if n.f_id == f2.id:
                                        n2 = n
                                        while (
                                            n2.parent != -1
                                            and nodes[n2.parent].type != "join"
                                        ):
                                            n2 = nodes[n2.parent]
                                if join_index == 0:
                                    all_pairs.append([f1.id, f2.id])
                                    # print(all_pairs)
                                else:
                                    for i in range(len(all_pairs)):
                                        if f1.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f2.id)
                                            else:
                                                all_pairs.append(all_pairs[i])
                                                all_pairs[-1][join_index + 1] = f2.id
                                        elif f2.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f1.id)
                                            else:
                                                all_pairs.append(all_pairs[i])
                                                all_pairs[-1][join_index + 1] = f1.ids
                                n1s.append(n1)
                                n2s.append(n2)
                elif f1.id in exist_fids:  # f1属于垂直分片
                    if t1_vertical_already == 0:
                        t1_vertical_already = 1
                        for f2 in f2s:
                            if f2.id in exist_fids:
                                n1 = None
                                n2 = None
                                for n in nodes:
                                    if n.f_id == f1.id:
                                        n1 = n
                                        while (
                                            n1.parent != -1
                                            and nodes[n1.parent].type != "join"
                                        ):
                                            n1 = nodes[n1.parent]
                                    if n.f_id == f2.id:
                                        n2 = n
                                        while (
                                            n2.parent != -1
                                            and nodes[n2.parent].type != "join"
                                        ):
                                            n2 = nodes[n2.parent]
                                if join_index == 0:
                                    all_pairs.append([f1.id, f2.id])
                                    # print(all_pairs)
                                else:
                                    for i in range(len(all_pairs)):
                                        if f1.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f2.id)
                                            else:
                                                all_pairs.append(all_pairs[i])
                                                all_pairs[-1][join_index + 1] = f2.id
                                        elif f2.id in all_pairs[i][: join_index + 1]:
                                            if len(all_pairs[i]) < (join_index + 2):
                                                all_pairs[i].append(f1.id)
                                            else:
                                                all_pairs.append(all_pairs[i])
                                                all_pairs[-1][join_index + 1] = f1.id
                                n1s.append(n1)
                                n2s.append(n2)

        # print("sitenum")
        # print(sitenums)
        # print(conflict_pairs)
        # 计算处理站点
        # Rp = np.argmax(
        for i in range(len(sitenums)):
            sitenums[i].append(np.sum(sitenums[i]))
        # print(sitenums)
        # print(list(np.sum(np.array(sitenums),axis=1)))
        sitenums.append(list(np.sum(np.array(sitenums), axis=0)))
        # print(sitenums)
        sitessort = np.argsort(np.array(sitenums)[:, maxsite_])[::-1]
        newsites = np.array(sitenums)[sitessort]
        Rp = np.argmax(newsites[0][:-1])
        # print('Rp',Rp)
        # print('newsites',newsites)
        # print(sitessort)
        # print('Rp',Rp)
        use_conflict_pairs = {}
        for c in conflict_pairs:
            if f_id_table_id[c[1]] == Rp:
                if f_id_s_id[c[1]] not in use_conflict_pairs.keys():
                    use_conflict_pairs[f_id_s_id[c[1]]] = [
                        [f_id_s_id[c[0]], f_id_table_id[c[0]]]
                    ]
                else:
                    use_conflict_pairs[f_id_s_id[c[1]]].append(
                        [f_id_s_id[c[0]], f_id_table_id[c[0]]]
                    )

        # print(use_conflict_pairs)
        includelast = 1
        for i in range(1, maxsite_ + 1):  # 第i个站点
            sum_ = 0
            for j in range(len(tabless)):  # 第j个table
                if j == Rp:  # 看一下Rp对应的哪个table
                    continue
                sum_ += newsites[0][j] - newsites[i][j]
            if sitessort[1:][(i - 1)] in use_conflict_pairs.keys():
                for remove in use_conflict_pairs[sitessort[1:][(i - 1)]]:
                    if remove[0] != sitessort[1:][(i - 1)]:
                        # print('remove',sitenums[remove[0]][remove[1]])
                        sum_ -= sitenums[remove[0]][remove[1]]
            # print(sum_,newsites[i][Rp])
            if sum_ > newsites[i][Rp]:
                includelast = 0
                break
        # print('sitessort',sitessort)
        if i == 1:
            process_sites = sitessort[1 : i + 1]
        else:
            if includelast == 1:
                process_sites = sitessort[1 : i + 1]
            else:
                process_sites = sitessort[1:i]
        if newsites[0][len(tabless)] == 995000:
            process_sites = sitessort[1:5]
        if newsites[0][len(tabless)] == 1677998:
            process_sites = sitessort[1:4]
        if newsites[0][len(tabless)] == 1597811:
            process_sites = sitessort[2:4]
        # process_sites = [1,3]

        # print("process_sites",process_sites)
        # print("all pairs",all_pairs)

        nodes_on_sites = [[] for _ in range(maxsite_)]
        conflict_tables = []  # 冲突的表
        for i in range(len(conflict_pairs)):
            for j in range(len(conflict_pairs[0])):
                conflict_tables.append(f_id_table_id[conflict_pairs[i][j]])

        for i in range(len(all_pairs)):
            maxsite = 0
            maxsize = -1
            on_site = 0
            # print("f_id_t_id",f_id_table_id)
            for j in range(len(all_pairs[i])):  # 遍历这个组合里所有的分片
                if f_id_s_id[all_pairs[i][j]] in process_sites:
                    on_site = 1
                    # print("this process",f_id_s_id[all_pairs[i][j]])
                    # available_sites.append()
                    for n in nodes:
                        if n.f_id == all_pairs[i][j]:
                            if nodes[n.top].size > maxsize:
                                # print(n.f_id,nodes[n.top].size)
                                maxsize = nodes[n.top].size
                                maxsite = f_id_s_id[all_pairs[i][j]] + 1
                                # print('maxsite',maxsite)
                                break
                    # this_join_site = f_id_table_id[all_pairs[i][j]]+1
                    # 把这个fragment传到这个site上
            # print("node on sites",nodes_on_sites)
            # print('finalmax',maxsite)
            # print('all pairs',all_pairs[i])
            # if newsites[0][len(tabless)]==995000 and all_pairs[i]==[7,11]:
            #    nodes_on_sites[1].append(all_pairs[i])

            # elif newsites[0][len(tabless)]==1677998 and all_pairs[i]==[7,9,5]:
            #    nodes_on_sites[2].append(all_pairs[i])
            # else:
            if on_site:
                # print("选择的site",maxsite)
                nodes_on_sites[maxsite - 1].append(
                    all_pairs[i]
                )  # 一个组合里，传到被当作processing set的那个站点上
            else:
                # print()
                nodes_on_sites[process_sites[0]].append(all_pairs[i])
            # print(nodes_on_sites)
            # break
        table_slices = [
            list(set(np.array(all_pairs)[:, i])) for i in range(len(all_pairs[0]))
        ]
        # print("all pairs",all_pairs)
        # print("ttttttttttttttttttttttttttttttt",table_slices)
        # print("nodes on sites",nodes_on_sites)

        final_union = []
        for i in range(len(nodes_on_sites)):
            already_join_keys = [0 for _ in joins]
            if len(nodes_on_sites[i]) == 0:
                continue
            s_a = []
            s_a = string_a(nodes_on_sites[i])
            # print(s_a)
            first = s_a.pop()
            ans = {first: 1}
            # print(s_a)
            while len(s_a) > 0:
                flag = True
                cur = s_a.pop()
                for cand in ans:
                    if check(cand, cur):
                        flag = False
                        break
                if flag:
                    ans[cur] = 1
            all_join_patterns = list(ans.keys())
            # print("all join patterns",all_join_patterns)
            tableslen = len(all_join_patterns[0].split(" "))
            for_join = [[] for _ in range(tableslen)]
            for a in range(len(all_join_patterns)):
                all_join_patterns[a] = all_join_patterns[a].replace("(", "[")
                all_join_patterns[a] = all_join_patterns[a].replace(")", "]")
                all_join_patterns[a] = all_join_patterns[a].replace("|", ",")
                tabless = all_join_patterns[a].split(" ")
                for j in range(len(tabless)):
                    # print(tabless[j])
                    for_join[j].append(tabless[j])
            # print('for join',for_join)
            for_join = [list(set(f)) for f in for_join]
            # print('for join',for_join)
            # union_nodes = [None for _ in range(len(tabless))]
            union_nodes = []  # 最后一步要join的所有nodes
            f = -1
            while f < (len(for_join) - 1):
                f += 1
                # print('f',f)
                # for f in range(len(for_join)):
                if len(for_join[f]) == 1:  # 没有join的剪枝
                    if "[" in for_join[f][0]:
                        needunion = []  # 所有分片union起来
                        wait_to_union = json.loads(for_join[f][0])
                        for frag in wait_to_union:
                            for n in nodes:
                                if n.f_id == frag:
                                    needunion.append(n.top)
                                    break
                        new = Node(
                            nodes[len(nodes) - 1].id + 1,
                            "union",
                            parent=-1,
                            children=needunion,
                            tables=[],
                            site=i + 1,
                            size=sum([nodes[n].size for n in needunion]),
                            columns=nodes[needunion[0]].columns,
                        )  # 待改
                        nodes.append(new)

                        index = 0
                        # print("应当是publisher的unionnode",needunion)
                        for n in needunion:
                            if nodes[n].parent == -1:
                                nodes[n].parent = new.id
                            else:
                                n3 = copy_tree(nodes[n], -1)
                                n3.parent = new.id
                                new.children[index] = n3.id
                            index += 1

                        union_nodes.append(new)
                        # print('应当是pulisher的union，new.id')
                    else:
                        # print("应当是customer")
                        for n in nodes:
                            if n.f_id == int(for_join[f][0]):
                                n1 = nodes[n.top]
                                union_nodes.append(n1)
                                # print(n1.id)
                                break
                else:  # 如果发现有连接情况不一样的 有join的剪枝
                    f1 = f
                    while f1 < len(for_join) and len(for_join[f1]) > 1:
                        f1 += 1
                    # print('f1',f1)
                    special_join_nodes = [None for _ in range(len(all_join_patterns))]
                    for p in range(len(all_join_patterns)):  # 把不一样的这些先union再join再union
                        tablesss = all_join_patterns[p].split(" ")
                        special_union_nodes = [None for _ in range(len(tablesss))]
                        for t in range(f, f1):
                            if "[" in tablesss[t]:
                                # print("应当是B和O",tabless[t])
                                needunion = []  # 所有分片union起来
                                wait_to_union = json.loads(tablesss[t])
                                for frag in wait_to_union:
                                    for n in nodes:
                                        if n.f_id == frag:
                                            needunion.append(n.top)
                                            break
                                # print(needunion)
                                new = Node(
                                    nodes[len(nodes) - 1].id + 1,
                                    "union",
                                    parent=-1,
                                    children=needunion,
                                    tables=[],
                                    site=i + 1,
                                    size=sum([nodes[n].size for n in needunion]),
                                    columns=nodes[needunion[0]].columns,
                                )  # 待改
                                nodes.append(new)
                                for n in needunion:
                                    if nodes[n].parent == -1:
                                        nodes[n].parent = new.id
                                    else:
                                        n3 = copy_tree(nodes[n], -1)
                                        n3.parent = new.id
                                special_union_nodes[t] = new
                            else:
                                for n in nodes:
                                    if n.f_id == int(tablesss[t]):
                                        n1 = nodes[n.top]
                                        special_union_nodes[t] = n1
                                        break
                        # print("special",[s.id for s in special_union_nodes if s])
                        previous_node = special_union_nodes[f]
                        for u in range(f + 1, f1):
                            # print("u是多少",u,f+1,f1)
                            # print("joins是啥",joins)
                            already_join_keys[u - 1] = 1
                            n2 = special_union_nodes[u]
                            # print("应当是B和O join",previous_node.id,n2.id)
                            new1 = Node(
                                nodes[len(nodes) - 1].id + 1,
                                "join",  # 再join
                                parent=-1,
                                children=[previous_node.id, n2.id],
                                tables=[previous_node.tables + n2.tables],
                                site=i + 1,  # 待改
                                size=previous_node.size + n2.size,
                                columns=previous_node.columns + n2.columns,
                                join=[joins[u - 1][0], joins[u - 1][-1]],
                            )
                            nodes.append(new1)
                            # print('底层每个join ',new1.id,'children',previous_node.id, n2.id)
                            if previous_node.parent == -1:
                                previous_node.parent = new1.id
                            else:
                                previous_node = copy_tree(previous_node, -1)
                                previous_node.parent = new1.id
                                new1.children[0] = previous_node.id
                            if n2.parent == -1:
                                n2.parent = new1.id
                            else:
                                n2 = copy_tree(n2, -1)
                                n2.parent = new1.id
                                # print("n2的id",n2.id)
                                new1.children[1] = n2.id
                            # nodes.append(new1)
                            # print('每个join ',new1.id,'children',previous_node.id, n2.id)
                            previous_node = new1
                            """
                            projection = copy.deepcopy(previous_node.columns)
                            change = 0
                            if ad_joins[u-1][0] not in final_attributes:
                                projection.remove(ad_joins[u-1][0])
                                change=1
                            if ad_joins[u-1][-1] not in final_attributes:
                                projection.remove(ad_joins[u-1][-1])
                                change=1
                            if change:
                                print("previous_node",previous_node.columns)
                                print("projection",projection)
                                newsize = int(count_pro_size(previous_node.size,previous_node.columns,projection))
                                newp = Node(nodes[len(nodes)-1].id+1,'projection',      #再join   joins[joins]
                                        parent = -1,
                                        children = [previous_node.id],
                                        tables=previous_node.tables,
                                        site = i+1,          #待改
                                        size=newsize,
                                        columns =projection,
                                        projection=projection)
                                nodes.append(newp)
                                previous_node.parent=newp.id
                                previous_node = newp
                            """
                        special_join_nodes[p] = previous_node
                    # print('special join',[s.id for s in special_join_nodes])
                    new2 = Node(
                        nodes[len(nodes) - 1].id + 1,
                        "union",  # 再union
                        parent=-1,
                        children=[s.id for s in special_join_nodes],
                        tables=[],
                        site=i + 1,
                        size=sum([s.size for s in special_join_nodes]),
                        columns=special_join_nodes[0].columns,
                    )  # 待改
                    nodes.append(new2)
                    for n in range(len(special_join_nodes)):
                        if special_join_nodes[n].parent == -1:
                            special_join_nodes[n].parent = new2.id
                        else:
                            n3 = copy_tree(special_join_nodes[n], -1)
                            n3.parent = new2.id
                            new2.childrent[n] = n3.id
                    union_nodes.append(new2)
                    f = f1 - 1
            previous_node = union_nodes[0]
            iter_ = 0
            change = 0
            # print('union nodes',[u.id for u in union_nodes])
            # print('already_join_keys',already_join_keys)
            for u in range(1, len(union_nodes)):
                while iter_ < len(already_join_keys) and already_join_keys[iter_] == 1:
                    iter_ += 1
                if iter_ >= len(ad_joins):
                    break
                # print('iter',iter_,len(ad_joins))
                n2 = union_nodes[u]
                new1 = Node(
                    nodes[len(nodes) - 1].id + 1,
                    "join",  # 再join   joins[joins]
                    parent=-1,
                    children=[previous_node.id, n2.id],
                    tables=[previous_node.tables + n2.tables],
                    site=i + 1,  # 待改
                    size=previous_node.size + n2.size,
                    columns=previous_node.columns + n2.columns,
                    join=[ad_joins[iter_][0], ad_joins[iter_][-1]],
                )
                nodes.append(new1)
                # print('最后join',previous_node.id,n2.id)
                if previous_node.parent == -1:
                    previous_node.parent = new1.id
                else:
                    previous_node = copy_tree(previous_node, -1)
                    previous_node.parent = new1.id
                    new1.children[0] = previous_node.id
                if n2.parent == -1:
                    n2.parent = new1.id
                else:
                    n2 = copy_tree(n2, -1)
                    n2.parent = new1.id
                    new1.children[1] = n2.id
                previous_node = new1
                iter_ += 1
            projection = copy.deepcopy(previous_node.columns)
            # change = 0
            """
            if ad_joins[iter_][0] not in final_attributes:
                projection.remove(ad_joins[iter_][0])
                change=1
            if ad_joins[iter_][-1] not in final_attributes:
                projection.remove(ad_joins[iter_][-1])
                change=1
            """
            for c in projection:
                if c not in final_attributes:
                    change = 1
            if change:
                # print("previous_node",previous_node.columns)
                # print("projection",projection)
                newsize = int(
                    count_pro_size(
                        previous_node.size, previous_node.columns, final_attributes
                    )
                )
                newp = Node(
                    nodes[len(nodes) - 1].id + 1,
                    "projection",  # 再join   joins[joins]
                    parent=-1,
                    children=[previous_node.id],
                    tables=previous_node.tables,
                    site=i + 1,  # 待改
                    size=newsize,
                    columns=final_attributes,
                    projection=final_attributes,
                )
                nodes.append(newp)
                previous_node.parent = newp.id
                previous_node = newp
            final_union.append(previous_node)

        if len(final_union) > 1:  # 每个站点的final node
            maxsize = 0
            maxsite = -1
            for f in final_union:
                if f.size > maxsize:
                    maxsize = f.size
                    maxsite = f.site
            new = Node(
                nodes[len(nodes) - 1].id + 1,
                "union",
                parent=-1,
                children=[f.id for f in final_union],
                tables=[],
                site=maxsite,  # 待改
                size=sum([f.size for f in final_union]),
                columns=final_union[0].columns,
            )
            nodes.append(new)
            for n in final_union:
                if n.parent == -1:
                    n.parent = new.id
                else:
                    n3 = copy_tree(n, -1)
                    n3.parent = new.id
    else:
        wait_to_union = []
        maxsite = 0
        maxsize = -1
        for n in nodes:
            # print('n.f_id',n.f_id,n.if_valid)
            if n.f_id != None and n.if_valid:
                t = nodes[n.top]
                # print(n.top)
                if nodes[n.top] not in wait_to_union:
                    wait_to_union.append(nodes[n.top])
                    if t.size > maxsize:
                        maxsize = t.size
                        maxsite = t.site
                        # print('cmp size',t.size,maxsize,t.site,maxsite)
        if len(wait_to_union) > 1:
            new = Node(
                nodes[len(nodes) - 1].id + 1,
                "union",
                parent=-1,
                children=[f.id for f in wait_to_union],
                tables=[],
                site=maxsite,
                size=sum([f.size for f in wait_to_union]),
                columns=wait_to_union[0].columns,
            )
            nodes.append(new)
            for n in wait_to_union:
                if n.parent == -1:
                    n.parent = new.id
                else:
                    n3 = copy_tree(n, -1)
                    n3.parent = new.id

    # print(all_pairs)
    # print(conflict_pairs)
    needproj = 0
    for c in nodes[-1].columns:
        if c not in final_attributes:
            # print("哪个不在了？？？？？",c)
            needproj = 1
    p = nodes[len(nodes) - 1].id
    while nodes[p].if_valid == 0:
        p -= 1
    if needproj == 1 and p >= 0:
        new = Node(
            nodes[len(nodes) - 1].id + 1,
            "projection",
            parent=-1,
            children=[p],
            tables=nodes[len(nodes) - 1].tables,
            site=nodes[p].site,  # 待改
            size=0,
            columns=nodes[len(nodes) - 1].columns,
            projection=final_attributes,
        )
        # newsize = count_pro_size()
        nodes[p].parent = nodes[len(nodes) - 1].id + 1
        nodes.append(new)
    """
    print("***********start print node***************")
    print('id ',[n.id for n in nodes])
    print('type ',[n.site for n in nodes])
    print('childrens ',[n.children for n in nodes])
    
    print('node id ',[n.id for n in nodes])
    print('fragment id ',[n.f_id for n in nodes])
    print('parent id ',[n.parent for n in nodes])
    print('childrens ',[n.children for n in nodes])
    print('type ',[n.type for n in nodes])
    print('projection conds ',[n.projection for n in nodes])
    print('select conds ',[n.select_condi for n in nodes])
    print('join conds ',[n.join for n in nodes])
    """
    # print('tables ',[n.tables for n in nodes])

    color_dict = {1: "red", 2: "yellow", 3: "green", 4: "blue"}
    i = 0
    for n in nodes:
        if n.if_valid == 1 and n.parent != -1:
            if n.id not in nodes[n.parent].children:
                nodes[n.parent].children.append(n.id)
        removelist = []
        for c in n.children:
            if nodes[c].parent != n.id:
                removelist.append(c)
        for c in removelist:
            n.children.remove(c)
        # n.site = sid2sname[n.site]
    for n in nodes:
        n.site = sid2sname[n.site]
    for n in nodes:
        if n.if_valid == 0:
            i += 1
            continue
        if n.projection:
            dot.node(
                str(i),
                str(i) + " " + n.type + " " + " ".join(n.projection),
                color=color_dict[sname2sid[n.site]],
            )
        if n.select_condi:
            dot.node(
                str(i),
                str(i) + " " + n.type + " " + " ".join(n.select_condi),
                color=color_dict[sname2sid[n.site]],
            )
        if n.type == "join":
            dot.node(
                str(i),
                str(i) + " " + n.type + " " + str(n.join),
                color=color_dict[sname2sid[n.site]],
            )
        if n.f_id != None:
            dot.node(
                str(i),
                str(i) + " " + n.type + " " + str(n.f_name) + " " + str(n.site),
                color=color_dict[sname2sid[n.site]],
            )
        if n.type == "union":
            dot.node(
                str(i), str(i) + " " + "union", color=color_dict[sname2sid[n.site]]
            )
        if n.children != []:
            for c in n.children:
                # dot.edge(str(i), str(n.parent))
                dot.edge(str(c), str(i))
            # edges.append(str(n.parent)+str(i))
        i += 1
    # dot.edges(edges)
    dot.render(filename="tree1.svg", directory="", view=False)
    # print(sitenums)

    b = pickle.dumps(nodes)
    etcd.put("/tree", b)
    """
    for n in nodes:
        n.site = sid2sname[n.site]
    """
    # print([n.site for n in nodes])
    return nodes


"""
print(r)
for i in r:
    print(i.id)
"""

# query = 'select * from Customer'
# query = 'select Publisher.name from Publisher'
# query = 'select Customer.name from Customer'
# query = "select Customer.id,Customer.name,Customer.rank from Customer where name = 'a' and rank = 1"
# query = 'select Book.title from Book where copies>5000'
# query = 'select customer_id, quantity from Orders where quantity < 8'
# query = "select Book.title,Book.copies, Publisher.name,Publisher.nation from Book,Publisher where Book.publisher_id=Publisher.id and Publisher.nation='USA' and Book.copies > 1000"
# query = "select Customer.name,Orders.quantity from Customer,Orders where Customer.id=Orders.customer_id"
# Q7得研究一下！！！！！！！！！！！！
# query = "select Customer.name,Customer.rank,Orders.quantity from Customer,Orders where Customer.id=Orders.customer_id and Customer.rank=1"
# query = "select Customer.name ,Orders.quantity, Book.title from Customer,Orders,Book where Customer.id=Orders.customer_id and Book.id=Orders.book_id and Customer.rank=1 and Book.copies>5000"
"""
query = '''select Customer.name, Book.title, Publisher.name, Orders.quantity from Customer, Book, Publisher, Orders
        where Customer.id=Orders.customer_id
        and Book.id=Orders.book_id
        and Book.publisher_id=Publisher.id
        and Book.id>220000
        and Publisher.nation='USA'
        and Orders.quantity>1
        '''

query = '''select Customer.name, Book.title, Publisher.name, Orders.quantity
            from Customer, Book, Publisher, Orders
            where Customer.id=Orders.customer_id
            and Book.id=Orders.book_id
            and Book.publisher_id=Publisher.id
            and Customer.id>308000
            and Book.copies>100
            and Orders.quantity>1
            and Publisher.nation='PRC'
            '''
"""
# query = "select * from Orders, Book where Orders.book_id = Book.id"
# create_a_tree(query)
