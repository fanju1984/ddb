#include "iostream"
#include <stdio.h>
#include <vector>
#include <string>
#include "../metadata/metadata.h"
#include "../executor/cpp/executor_multi.h"
#include <map>
#include <sys/time.h>
#include <time.h>
#include <fstream>

using namespace std;

// // should be delete
// // fragment 
// struct FragDef {
// 	int id;
//     int site;
//     string column; //该分片涉及到的column，如果是多个，则用逗号分隔
// 	string condition; //分片条件
//     int size; //记录该分片的大小，以byte为单位
// };
// // table-fragment
// struct Fragment {
// 	string name;
// 	string fragtype; // H/V 
// 	int fragnum;
// 	vector<FragDef> frags;
// };
// // should be delete

// Do not need metadata

// For SELECT // 

// For SELECT END //
map<string,Fragment> GetETCDFragment();
map<string,string> GetETCDTableKey();
string GetBetween(string sql_statement, string start, string end);
string GetAfter(string sql_statement, string start);
string GetBefore(string sql_statement, string end);
string GetExactAfter(string sql_statement, string start);
vector<string> GetList(string line, string split, string stop);
void Traverse(vector<string> input);
string Link(vector<string> input, string devide);
string GetTableName(string sql_statement);
vector<string> GetTableList(string TableName);
string GetSelectColumnName(string sql_statement);
vector<string> GetSelectColumnList(string sql_statement);
string GetCondition(string sql_statement);
vector<string> GetConditionList(string sql_statement);
string GetTableFromColumn(string column, vector<string> TableList);
vector<string> GetColumnFromCondition(string condtion, vector<string> TableList);
vector<string> GetColumnListFromConditionList(vector<string> ConditionList, vector<string> TableList);
vector<string> GetAllColumnList(string sql_statement);
vector<string> GetAllDifferentColumnList(string sql_statement);
vector<string> GetAllDifferentTreeNodeColumnList(string sql_statement, string table_1, string treenode1, string table_2, string treenode2);
vector<string> GetAllTreeNodeCollumnList(string sql_statement, string table_1, string treenode1, string table2, string treenode2);
vector<string> GetAllData(string sql_statement);
int GetTCLoc(string table, string column);
string GetPureColumnFromColumn(string column);
string Link(vector<string> input, string devide);
void Traversefrags(vector<FragDef> frags);
void TraverseFragment(Fragment frags);
string GetTableListFromInsert(string sql_statement);
vector<string> GetColumnListFromInsert(string sql_statement,string table_name);
vector<string> GetValueListFromInsert(string sql_statement);
bool JudgeHit(vector<string> BigList, string oneitem);
bool JudgeHit2 (vector<string> FirstList, vector<string> SecondList);
int GetLocateHit(vector<string> BigList, string oneitem);
bool Judge(vector<string> condition_list, vector<string> condition_column_list, vector<string> insert_column_list, vector<string> insert_value_list);
string GetValue(vector<string> column_in_frag_list, vector<string> value_list, vector<string> column_list);
string GetInsetItem(vector<string> column_list, vector<string> value_list);
void trim(string &s);
void TraverseGDD(GDD gdd);
void TraverseGDDCol(vector<ColumnDef> cols);

vector<string> GetCollumnListOfTable(string table);
string Getall_collumn(string table_1,string treenode1,string table_2,string treenode2);
string GetTreeNode(int treeid,int nodeid);
vector<string> GetAllDifferentCollumnListOfTable(string sql_statement, string table_name);
vector<string> GetCleanList(vector<string> input);
map<string,GDD> GetETCDGDD();
