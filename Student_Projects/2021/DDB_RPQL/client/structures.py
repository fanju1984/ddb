from typing import List

class Attribute:
    def __init__(self, table: str, attr: str, if_join_attr: bool=False, f_id: int=None):
        self.table = table       #table name
        self.attr = attr        #attribute name
        self.f_id = f_id        #fragment id if this attribute is a condition for vertical fragment
        self.if_join_attr = if_join_attr #if this attribute is included in a join condition
        #self.s_id = s_id    

class Selection:
    def __init__(self, attr: Attribute, operator: str, f_id: int, s_id:int, 
                 num_value: float=None, str_value: str=None):
        self.attr = attr       #attribute name
        self.operator = operator       #operator > = < >= <=
        self.f_id = f_id             #fragment id if this select is a condition for 
                                     #horizontal fragment, start from 0
        self.s_id = s_id           #site id if this select is a condition for horizontal fragment, 
                                    #start from 1, 1,2,3,4
        self.num_value = num_value    #if the value is a number
        self.str_value = str_value    #if the value is a string

class Join:
    def __init__(self, leftattr: Attribute, rightattr: Attribute):
        self.leftattr = leftattr   #join condition left attribute
        self.rightattr = rightattr    #join condition right attribute

class Fragment:
    def __init__(self, id:int, table: str, s_id: int, size: int, hor_or_ver: int, 
                hori_condi: List[Selection]=None, verti_condi: List[Attribute]=None):
        self.table = table       #the table name this fragment is in
        self.id = id            #the fragment id, start from 0
        self.s_id = s_id        #the site id, start from 1, 1,2,3,4
        self.size = size        #total bytes of data in this fragment
        self.hor_or_ver = hor_or_ver       #horizontal of vertical fragment
        self.hori_condi = hori_condi       #horizontal fragment conditions
        self.verti_condi = verti_condi     #vertical fragment attributes

class Table:
    def __init__(self, table:str, frag_list:List[Fragment], attributes:List[str]):
        self.table=table                 #table name
        self.frag_list = frag_list       #the fragments of this table
        self.attributes = attributes     #attributes of this table

class Node:
    def __init__(self, index: int, type:str, parent:int, children:List[int], tables:List[str], 
                 site:str, size:int, columns:List[str], if_valid:bool=True, f_id:int=None, 
                 f_name:str=None, select_condi: List[List[str]]=None, projection:List[str]=None, 
                 join:List[str]=None, top:int=None):
        self.id = index                   #node id, start from 0
        self.type = type                  #node type: fragment, select, projection, join, union
        self.parent = parent             #the parent node id, default -1
        self.children = children         #the children node ids, defalut []
        self.tables = tables             #names of all tables included in this node
        self.site = site                 #which site this node is in
        self.size = size                 #total bytes of data in this node
        self.if_valid = if_valid          #if this node has been pruned
        self.columns = columns            #names of all attributes included in this node
        self.f_id = f_id                  #if type is 'fragment', the fragment id
        self.f_name = f_name              #if type is 'fragment', the table name
        self.select_condi = select_condi       #if type is 'select', the select condition
        self.projection = projection      #if type is 'projection', the project attributes
        self.join = join                 #if type is join, the two join keys
        self.top = top                 #if type is 'fragment', the node id after select and project