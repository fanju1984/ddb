import enum
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

from os import stat
from structures import Attribute,Selection,Join,Fragment,Node,Table
import sqlparse
import copy
from graphviz import Digraph
import math
import numpy as np
import re
import json
import pickle

etcd = Etcd_S(host=getip(), port=2379)
b = etcd.get("/tree")[0]
r = pickle.loads(b)
print(r)
for i in r:
    print(i.id)
