#include "Parse_LOAD.h"
using namespace std;

// load sql contains 3 sub sentence parser parse the sql seperately
// sentence 1: LOCAL CREATE TABLE
// sentence 2: LOCAL LOAD ALLDATA
// sentence 3: LOCAL LOAD LOCALDATA
// the sql is parsed differently according to the fragmenttype

// int main() {
//     // H
//     string sql_statement;
//     string create_sql_yq;
//     string load_sql_yq;
//     string main_name;
//     vector<string> sql_statements;
//     vector<string> sitenames ;
//     vector<string> sqls ;
//     vector<string> table_names ;

//     sql_statement = "LOCAL CREATE TABLE Publisher (id int(6) key, name char(100), nation char(3)) ON SITE 1;";
//     create_sql_yq = GetLocalCreate(sql_statement);
//     cout << create_sql_yq << endl;
//     sql_statement = "LOCAL LOAD ALLDATA TO TABLE Publisher FROM LOCAL FILE the path;";
//     load_sql_yq = GetLoadSql(sql_statement);
//     cout << load_sql_yq << endl;
//     main_name = GetTableFromLocalLoad(sql_statement);
//     cout << main_name << endl;
//     sql_statements.clear();
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Publisher BY H WITH ( id<104000 nation='PRC') TO Publisher_1 ON SITE 1;";
//     sql_statements.push_back(sql_statement);
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Publisher BY H WITH ( id<104000 nation='USA') TO Publisher_2 ON SITE 2;";
//     sql_statements.push_back(sql_statement);
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Publisher BY H WITH ( id>=104000 nation='PRC') TO Publisher_3 ON SITE 3;";
//     sql_statements.push_back(sql_statement);
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Publisher BY H WITH ( id>=104000 nation='USA') TO Publisher_4 ON SITE 4;";
//     sql_statements.push_back(sql_statement);
//     sitenames = GetSiteNames(sql_statements);
//     sqls = GetSqls(sql_statements);
//     table_names = GetTableNames(sql_statements);
//     Traverse(sitenames);
//     Traverse(sqls);
//     Traverse(table_names);
//     // string result = Data_Load_Execute(create_sql_yq,load_sql_yq,main_name,sitenames,sqls,table_names); // call yq's function
    
//     // V
//     sql_statement = "LOCAL CREATE TABLE Customer (id int(6) key, name char(100), rank int(6)) ON SITE 1;";
//     create_sql_yq = GetLocalCreate(sql_statement);
//     cout << create_sql_yq << endl;
//     sql_statement = "LOCAL LOAD ALLDATA TO TABLE Customer FROM LOCAL FILE the path;";
//     load_sql_yq = GetLoadSql(sql_statement);
//     main_name = GetTableFromLocalLoad(sql_statement);
//     cout << load_sql_yq << endl;
//     cout << main_name << endl;
//     sql_statements.clear();
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Customer BY V WITH ( id name) TO Customer_1 ON SITE 1;";
//     sql_statements.push_back(sql_statement);
//     sql_statement = "LOCAL LOAD LOCALDATA TABLE Customer BY V WITH ( id rank) TO Customer_2 ON SITE 2;";
//     sql_statements.push_back(sql_statement);
//     sitenames = GetSiteNames(sql_statements);
//     sqls = GetSqls(sql_statements);
//     table_names = GetTableNames(sql_statements);
//     Traverse(sitenames);
//     Traverse(sqls);
//     Traverse(table_names);
//     // string result = Data_Load_Execute(create_sql_yq,load_sql_yq,main_name,sitenames,sqls,table_names); // call yq's function

//     return 0;
// }
// local create 
int GetSite(string sql_statement) {
    int site = stoi(GetBetween(sql_statement, "SITE", ";"));
    return site;
}
string GetLocalCreate(string sql_statement) {
    string localcreate;
    localcreate = GetBetween(sql_statement,"LOCAL"," ON") ;
    return localcreate;
}
string GetSitename(string sql_statement) {
    int site = GetSite(sql_statement);
    return "s" + to_string(site);
}
// local load all data
string GetTableFromLocalLoad(string sql_statement) {
    return GetBetween(sql_statement,"TABLE"," FROM");
}
string GetPathFromLocalLoad(string sql_statement) {
    return GetBetween(sql_statement, "FILE",";");
}
string GetFragType(string sql_statement){
    string type;
    type = GetBetween(sql_statement, "BY", "WITH");
    return type.substr(0,1);
}

string GetLoadSql(string sql_statement){
    string table_name  = GetTableFromLocalLoad(sql_statement);
    string path = GetPathFromLocalLoad(sql_statement);
    return "load data infile '" + path + "' into table " + table_name;
}
// local load local data
string GetSelectSqlFromLoadLocalData(string sql_statement) {
    string frag_type = GetFragType(sql_statement);
    string to_table = GetBetween(sql_statement, "TO", "ON");
    string from_table = GetBetween(sql_statement, "TABLE", "BY");
    string select_sql;
    if (frag_type == "H") {
        string condition = GetBetween(sql_statement, "(",")");
        vector<string> condition_list = GetList(condition," ",")");
        condition = Link(condition_list, " AND ");
        select_sql = "select * FROM " + from_table + "WHERE " + condition ;
    }
    else if (frag_type == "V") {
        string condition = GetBetween(sql_statement, "(",")");
        vector<string> condition_list = GetList(condition," ",")");
        condition = Link(condition_list, ", ");
        select_sql = "SELECT " + condition + " FROM " + from_table; 
    }
    else {
        cout << "INVALID FRAGMENT TYPE " << endl;
        return "ERROR";
    }
    // cout << select_sql << endl;
    return select_sql;
}
string GetToTable(string sql_statement) {
    string to_table = GetBetween(sql_statement, "TO", " ON");
    return to_table;
}

vector<string> GetSiteNames(vector<string> sql_statements) {
    vector<string> site_names;
    site_names.clear();
    for (int i = 0; i < sql_statements.size(); i++) {
        site_names.push_back(GetSitename(sql_statements[i]));
    }
    return site_names;
}
vector<string> GetSqls(vector<string> sql_statements) {
    vector<string> sqls;
    sqls.clear();
    for (int i = 0; i < sql_statements.size(); i++) {
        sqls.push_back(GetSelectSqlFromLoadLocalData(sql_statements[i]));
    }
    return sqls;
}
vector<string> GetTableNames(vector<string> sql_statements) {
    vector<string> table_names;
    table_names.clear();
    for (int i = 0; i < sql_statements.size(); i++) {
        table_names.push_back(GetToTable(sql_statements[i]));
    }
    return table_names;
}
