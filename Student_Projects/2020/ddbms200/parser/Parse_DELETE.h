#include "Tools.h"

string GetSelectFromDelete(string sql_statement);
vector<string> GetDeleteSqlH(string sql_statement, Fragment fragment);
vector<string> GetDeleteSiteH(string sql_statement, Fragment fragment);
vector<string> GetsqlDelete(string sql_statement);
vector<string> GetsitenameDelete(string sql_statement);
vector<string> GetColumnListFromDelete(string sql_statement);
string GetTableListFromDelete(string sql_statement);
vector<string> GetValueListFromDelete(string sql_statement);
bool JudgeDelete(vector<string> condition_list, vector<string> condition_column_list, vector<string> insert_column_list, vector<string> insert_value_list);
string GetSelectSql(string sql_statement);