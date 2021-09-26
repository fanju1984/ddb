#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "./mysql_connector.h"
#include "../../transfer/transfer/transfer.h"
#include <thread>
#include <future>
#include <utility>

/* 这是为了记录每次最多并行多少线程，受到机器最大并行数量的限制，
所以实质上应当取max(机器允许的最大并行数量，数据库站点数量)，
但事实上后面如果有递归执行，可能会直接超出机器能力，
而且直接写在这里也有点笨，size算出来然后在内部申请可以更省空间 */
#define MAXTHREAD 4

/* 在每个site实例上部署的时候这个值不同 */
#define LOCALSITE "s3"
#define LOCALSITE2 "s4"


// 现在定义一下时间和数据传输量记录的结构
// 也用树结构记录每个节点花费的时间和数据传输量
struct exec_node{
    int node_id; // 原来树中该节点对应的ID
    double time_spend; // 执行对应节点所花的时间，单位为秒
    int volume; // 该节点上结果的数据量，单位为比特
};
struct exec_tree{
    int tree_id; // 应当与它执行的树的ID一致
    vector<exec_node> nodes; // 应当与它执行的树的node数量一致
};


// string Data_Load_Execute(string create_sql, string load_sql, vector<string> sitenames, vector<string> sqls, vector<string> table_names){
//     return "x rows imported, y seconds used."
// }
// 这里是一条一条给我的？

/* for循环内原先的内容被封装为另外一个函数，
输入sitenames, sqls, table_names, 输出String - "xx rows imported on site x.\n" 或者 "FAIL on site x.\n" 
最后三个参数是为了传递主函数给每个线程的对应变量预留的空间，而不是传递值 */
void Data_Load_Thread(string site, string frag_sql, string frag_name, std::promise<string> &resultObj);

/* 本函数用于执行整个load流程，输入本地创建和导入表的SQL语句，本地表的表名，站点列表，
分片select语句列表和分片表名列表，返回
"x1 rows imported on site 1.
 x2 rows imported on site2.
 y seconds used."
 或者"FAIL" */
string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names);
