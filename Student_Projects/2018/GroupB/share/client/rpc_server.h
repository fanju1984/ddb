//
// Created by tommenx on 2018/10/27.
/* 在每个客户端上需要启动 RPC Server 的服务，监听在指定的端⼝上，等待 其他节点的调⽤*/
//

#ifndef DDBDEMO_RPC_SERVER_H

#include "rpc/server.h"
#include "local_sql.h"
#define DDBDEMO_RPC_SERVER_H

void startServer(int port);


#endif //DDBDEMO_RPC_SERVER_H
