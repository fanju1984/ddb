#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <ctime>
#include "./mysql_connector.h"
#include "../../transfer/transfer/transfer.h"
#include <thread>
#include <future>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace std;

/* 这是为了记录每次最多并行多少线程，受到机器最大并行数量的限制，
所以实质上应当取max(机器允许的最大并行数量，数据库站点数量)，
但事实上后面如果有递归执行，可能会直接超出机器能力，
而且直接写在这里也有点笨，size算出来然后在内部申请可以更省空间 */
#define MAXTHREAD 4

/* 在每个site实例上部署的时候这个值不同 */
#define LOCALSITE "s2"
#define LOCALSITE2 "sn"

/* 晓桐定义的树结构 */
// struct NODE {
// 	int id;
// 	vector<int> child;
// 	int parent;
// 	string sql_statement;
// 	int site;
// };
// struct TREE {
//     int tree_id;
//     int root;
//     vector<NODE> Nodes;
// };

// 现在定义一下时间和数据传输量记录的结构
// 也用树结构记录每个节点花费的时间和数据传输量
// struct exec_node{
//     int node_id; // 计划树中该节点对应的ID
//     double time_spend; // 执行对应节点所花的时间，单位为秒
//     size_t volume; // 该节点上结果的数据量，单位为比特
//     string res; //该节点上结果的执行情况，OK或者FAIL
//     vector<int> child; //该节点的孩子，与计划树一致
// 	int parent; //该节点的父亲，与计划树一致
//     int site; //该节点的site，与计划树一致
// };
// struct exec_tree{
//     int tree_id; // 应当与它执行的树的ID一致
//     int root; // 为了与原来的树保持一致
//     vector<exec_node> Nodes; // 应当与它计划树的node数量一致
// };

/* for循环内原先的内容被封装为另外一个函数，
输入sitenames, sqls, table_names, 输出String - "xx rows imported on site x.\n" 或者 "FAIL on site x.\n" 
最后三个参数是为了传递主函数给每个线程的对应变量预留的空间，而不是传递值 */
void Data_Load_Thread(string site, string frag_sql, string frag_name, std::promise<string> &resultObj);

/* 本函数供parser调用
本函数用于执行整个load流程，输入本地创建和导入表的SQL语句，本地表的表名，站点列表，
分片select语句列表和分片表名列表，返回
"x1 rows imported on site 1.
 x2 rows imported on site2.
 y seconds used."
 或者"FAIL" */
string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names);

/* 把int型的节点名称（1）转化为字符串型（"s1"） */
string site_to_string(int site_num);

/* 通过系统stat结构体获取文件大小，单位bytes，size_t为长整型，若要打印占位符为%ld */
size_t get_filebytes(const char *filename);

/* 输入一个计划树结构，返回这棵树的根节点 */
NODE get_root(TREE tree);

/* 输入一个执行树结构，返回这棵树的根节点 */
exec_node get_root(exec_tree tree);

/* 输入一个树结构和节点id，返回对应节点 */
NODE get_node(TREE tree, int node_id);

/* 输入一个树结构和节点id，返回以对应节点为根节点的子树 */
TREE get_sub_tree(TREE tree, int node_id);

/* RPC_Data_Select_Execute的可并行化版本，合并前先注释掉方便调试 */
void RPC_Data_Select_Execute_Thread(TREE tree, string site, std::promise<exec_tree> &resultObj);

/* Data_Select_Execute的可并行化版本 */
void Data_Select_Execute_Thread(TREE tree, std::promise<exec_tree> &resultObj);

/* 本函数供parser调用
用于递归执行整个select流程，输入一棵查询计划树，返回对应的执行结果树 
至于查询到的数据结果，从tree与node的id便可以推测得到结果表的名字，
mysql_connector.h里面也提供了根据结果表名字打印结果的函数void my_mysql_res_print(string my_res);
注意此版本非并行化版本，仅供RPC_Data_Select_Execute或者parser调用（递归的开始），内部调用的均为可并行化版本 */
exec_tree Data_Select_Execute(TREE tree);

/* for循环内原先的内容被封装为另外一个函数，
输入sitenames, sqls, 输出String - "OK on site x.\n" 或者 "FAIL on site x.\n" 
最后三个参数是为了传递主函数给每个线程的对应变量预留的空间，而不是传递值 */
void Data_Insert_Delete_Thread(string site, string frag_sql, std::promise<string> &resultObj);

/* 本函数供parser调用
本函数用于执行整个Insert或Delete流程，输入站点列表，分片sql语句列表，返回
"OK/FAIL on site s1.
 OK/FAIL on site s2.
 y seconds used." */
string Data_Insert_Delete_Execute(vector<string> sitenames, vector<string> sqls);