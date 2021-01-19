#ifndef _TRANSFER_H_
#define _TRANSFER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <grpcpp/grpcpp.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "transfer.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientWriter;
using grpc::ClientReader;
using grpc::Status;
using transfer::Stmt1;
using transfer::Stmt2;
using transfer::Reply;
using transfer::Chunk;
using transfer::TMPFile;
using transfer::MTree;
using transfer::ETree;
using transfer::MNode;
using transfer::ENode;
using transfer::Transfer;

using namespace std;
#define CHUNK_SIZE 1024 * 1024
#define TMPPATH "/mnt/d/ddbms200tmp/"

/* 晓桐定义的树结构 */
struct NODE {
	int id;
	vector<int> child;
	int parent;
	string sql_statement;
	int site;
};
struct TREE {
    int tree_id;
    int root;
    vector<NODE> Nodes;
};

// 现在定义一下时间和数据传输量记录的结构
// 也用树结构记录每个节点花费的时间和数据传输量
struct exec_node{
    int node_id; // 计划树中该节点对应的ID
    double time_spend; // 执行对应节点所花的时间，单位为秒
    size_t volume; // 该节点上结果的数据量，单位为比特
    string res; //该节点上结果的执行情况，OK或者FAIL
    vector<int> child; //该节点的孩子，与计划树一致
	int parent; //该节点的父亲，与计划树一致
    int site; //该节点的site，与计划树一致
};
struct exec_tree{
    int tree_id; // 应当与它执行的树的ID一致
    int root; // 为了与原来的树保持一致
    vector<exec_node> Nodes; // 应当与它计划树的node数量一致
};

// bool Transfer(int site,string data);
class TransferClient
{
public:
    TransferClient(std::shared_ptr<Channel> channel) : stub_(Transfer::NewStub(channel)){};
    string local_I_D(string sql, int site);
    string local_L(string sql1, string sql2, int site);
    string local_S(string sql, string file_name, int site);
    string local_T_L(string tmp_data, int site);
    exec_tree Data_S_E(TREE tree, int site);
private:
    unique_ptr<Transfer::Stub> stub_;
};

typedef struct webInfo{
    string IP;
    string RPC_PORT;
    string ETCD_PORT;
}WEB;

extern WEB site_info[4];

// RPC 调用接口声明
// 在目标site 实现insert/delete操作
string RPC_local_Insert_Delete(string sql, string site);
// 在目标site 实现Load操作
string RPC_local_Load(string sql_create, string sql_load, string site);
// 在目标site 实现select操作，并传回结果
string RPC_Local_Select(string sql, string res_name, string site);
// 向目标site 传输数据，并传回结果
string RPC_Local_Tmp_Load(string tmp_data, string site);
// 传输执行树并传回结果
exec_tree RPC_Data_Select_Execute(TREE tree, string site);
// 获取目标site 的文件
string RPC_GET_FILE(string filename, string site);

#endif /*_TRANSFER_H_*/