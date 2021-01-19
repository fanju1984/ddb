#include "Tools.h"
// #include "../executor/cpp/executor_multi.h"

// struct NODE {
//     int id;
//     vector<int> child;
//     int parent;
//     string sql_statement;
//     int site;
// };
struct TABLE{
    string table_name;
    int node_id;
};
// struct TREE {
//     int tree_id;
//     int root;
//     vector<NODE> Nodes;
// };
struct JOIN {
    string join_condition;
    vector<string> table_list;
    vector<int> node_list;
    int visit = 0;
};

struct TCC {
    string table_name;
    string fratype;
    vector<string> condition_list;
    vector<string> column_list;
};
// vector<string> GetCollumnsSelect(vector<string> table_list, vector<TCC> TCCList)

vector<string> GetCollumnsSelect(vector<string> table_list,vector<TCC> TCCList);
TCC GetTCC(string table_name, vector<string> column_list, vector<string> select_list);
vector<TCC> GetTCCList(vector<string> table_list, vector<string> column_list, vector<string> select_list);
vector<string> GetSelectConditionList(vector<string> condition_list, vector<string> table_list);
vector<string> GetJoinConditionList(vector<string> condition_list, vector<string> table_list);
bool JudgeNotin(vector<string> BigList, string item);
bool JudgeNotinInt(vector<int> BigList, int item);
void TraverseTCCList(vector<TCC> TccList);
string GetTableFromSelectCondition(string select_condition);
vector<string> GetTableFromJoinCondition(string join_condition);
vector<int> GetNodeListFromTableList(vector<string> table_in_join_condition, vector<TABLE> TableList);
vector<TCC> GetTCCListTest(string sql_statement);
// vector<NODE> GetData(string sql_statement, int treeid);
TREE SELECT(string sql_statement, int treeid);
void TraverseTree(vector<NODE> tree);
void TraverseInt(vector<int> intlist);
void TraverseTableList(vector<TABLE> TableList);
void TraverseTableMap(map<string,int> TableMap);
vector<string> GetKeyofTable(string table_name);
vector<string> GetFullKeyofTable(string table_name);
string Get_join_condition(int treeid, int a, int b, string key);
TREE SELECTLocal(string sql_statement,int treeid,map<string,Fragment> FragmentMap,map<string,string> TableKeyMap);
vector<string> GetLeafConditionList(vector<string> condition_list);
vector<string> GetInerColumnList(vector<string> column_list, int treeid, int nodeid);
vector<string> GetCollumnFromSql(string sql_statement);
vector<string> GetTableCollumnFromSql(string sql_statement);
void DrawTree(vector<NODE> Nodes);