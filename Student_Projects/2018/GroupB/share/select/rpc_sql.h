#include "rpc/client.h"
#include <iostream>
#include <string>
#include <vector>
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool RPCExecute(string ip,string db_name,string stmt);
int RPCExecuteUpdate(string ip,string db_name,string stmt);
string RPCExecuteQuery(string ip,string db_name,string stmt);
bool RPCExecuteIndsertTable(string ip,string db_name,string stmt);
bool RPCInsertFromSelect(string ip1,string ip2,string tb1,string tb2,string db1,string db2);
