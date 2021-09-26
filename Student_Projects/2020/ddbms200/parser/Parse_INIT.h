#include "Tools.h"

using namespace std;
// Create table
string InitGetTableCreateTable(string sql_statement);
vector<ColumnDef> InitGetColumnsCreateTable(string sql_statement);
GDD InitGetGDDCreateTable(string sql_statement);
// Create Fragment
string InitGetTableCreateFragmentation(string sql_statement);
string InitGetFragmentTypeCreateFragmentation(string sql_statement);
vector<FragDef> InitGetFragDefCreateFragmentation(string sql_statement);
int InitGetFragNumCreateFragmentation(string sql_statement);
Fragment InitGetFragmentCreateFragment(string sql_statement);
string InitGetFragsColumn(string condition);
string InitGetColumnFromCondition(string condition);