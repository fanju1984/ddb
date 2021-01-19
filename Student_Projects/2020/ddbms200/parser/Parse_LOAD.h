#include "Tools.h"

using namespace std;

// local create
int GetSite(string sql_statement);
string GetLocalCreate(string sql_statement);

string GetSitename(string sql_statement);
// local load ALLDATA
string GetPathFromLocalLoad(string sql_statement);
string GetTableFromLocalLoad(string sql_statement);
string GetFragType(string sql_statement);

string GetLoadSql(string sql_statement);
// local select and load
string GetSelectSqlFromLoadLocalData(string sql_statement);
string GetToTable(string sql_statement);

vector<string> GetSiteNames(vector<string> sql_statements);
vector<string> GetSqls(vector<string> sql_statements);
vector<string> GetTableNames(vector<string> sql_statements);
