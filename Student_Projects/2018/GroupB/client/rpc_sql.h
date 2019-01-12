//需要将所要执行的操作绑定在 RPC 的服务中，其中包括 execute，query 等
#include "rpc/client.h"
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool RPCExecute(string ip,int port,string db_name,string stmt);
bool RPCExecute1(string ip,int port,string db_name,string stmt);
int RPCExecuteUpdate(string ip,int port,string db_name,string stmt);
string RPCExecuteQuery(string ip,int port,string db_name,string stmt);
bool RPCExecuteIndsertTable(string ip,int port,string db_name,string stmt);
bool RPCInsertFromSelect(string ip1,int port1,string ip2,int port2,string tb1,string tb2,string db1,string db2);
