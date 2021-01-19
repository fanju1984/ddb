#include "Parse_INSERT.h"
using namespace std;

// int main() {
//     string sql_statement;
//     vector<string> sitenames;
//     vector<string> sql;

//     sql_statement = "INSERT INTO TABLE customer ( id,name,rank ) VALUES ( 300001,'Xiaoming',1 );";
//     sitenames = GetsitenameInsert(sql_statement);
//     sql = GetsqlInsert(sql_statement);
//     cout << "sql_statement :" << sql_statement << "/" << endl;
//     Traverse(sitenames);
//     Traverse(sql);
//     sql_statement = "INSERT INTO TABLE publisher ( id,name,nation ) VALUES ( 104001,'High Education Press','PRC' );";
//     sitenames = GetsitenameInsert(sql_statement);
//     sql = GetsqlInsert(sql_statement);
//     cout << "sql_statement :" << sql_statement << "/" << endl;
//     Traverse(sitenames);
//     Traverse(sql);
//     sql_statement = "INSERT INTO TABLE book ( id,title,authors,publisher_id,copies ) VALUES ( 205001,'DDB','Oszu',104001,100 );";
//     sitenames = GetsitenameInsert(sql_statement);
//     sql = GetsqlInsert(sql_statement);
//     cout << "sql_statement :" << sql_statement << "/" << endl;
//     Traverse(sitenames);
//     Traverse(sql);
//     sql_statement = "INSERT INTO TABLE orders ( customer_id,book_id,quantity ) VALUES ( 300001,205001,5 );";
//     sitenames = GetsitenameInsert(sql_statement);
//     sql = GetsqlInsert(sql_statement);
//     cout << "sql_statement :" << sql_statement << "/" << endl;
//     Traverse(sitenames);
//     Traverse(sql);
//     sql_statement = "INSERT INTO TABLE customer ( id,name,rank ) VALUES ( 300002,'Xiaohong',1 );";
//     sitenames = GetsitenameInsert(sql_statement);
//     sql = GetsqlInsert(sql_statement);
//     cout << "sql_statement :" << sql_statement << "/" << endl;
//     Traverse(sitenames);
//     Traverse(sql);
//     return 0;
// }

vector<string> GetsqlInsert(string sql_statement) {
    vector<string> sql;
    sql.clear();
    string table_name;
    vector<string> column_list;
    vector<string> value_list;
    table_name = GetTableListFromInsert(sql_statement);
    column_list = GetColumnListFromInsert(sql_statement,table_name);
    value_list = GetValueListFromInsert(sql_statement);
    
    // metadata //
    map<string,Fragment> FragmentMap = GetETCDFragment();
    map<string,string> TableKeyMap = GetETCDTableKey();

    Fragment fragment = getFragFromEtcd(table_name);
    // Fragment fragment = FragmentMap[table_name];
    string type = fragment.fragtype;
    vector<FragDef> frags = fragment.frags;
    vector<string> value_list_get;
    vector<string> column_list_get;
    if (type == "H"){
        for (int i = 0; i < frags.size(); i++){
            string column = frags[i].column;
            vector<string> column_in_frag_list = GetList(column,",",";");
            string condition = frags[i].condition;
            vector<string> condition_list = GetList(condition, "AND",";");
            if (JudgeHit2(column_in_frag_list,column_list)){
                cout << "JudgeHit2 !" << endl;
                if (Judge(condition_list,column_in_frag_list,column_list,value_list)) {
                    cout << "Judge!" << endl;
                    sql.push_back("INSERT INTO " + table_name + "_" + to_string(frags[i].site) + " " + GetInsetItem(column_list,value_list));
                }
            }
        }
    }
    else if (type == "V") {
        for (int i = 0; i < frags.size(); i++) {
            string column = frags[i].column;
            cout << "VVVVV" << endl;
            cout << "column :" << column << "/" << endl;
            vector<string> column_in_frag_list = GetList(column,",",";");
            cout << "column_in_frag_list :" << endl;
            Traverse(column_in_frag_list);
            cout << "column_list :" << endl;
            Traverse(column_list);
            if (JudgeHit2(column_in_frag_list, column_list)) {
                column_list_get.clear();
                value_list_get.clear();
                for (int j = 0; j < column_in_frag_list.size(); j++) {
                    int loc = GetLocateHit(column_list,column_in_frag_list[j]);
                    cout << "loc :" << loc << "/" << endl;
                    column_list_get.push_back(column_list[loc]);
                    value_list_get.push_back(value_list[loc]);
                }
                column_list_get = GetPure(column_list_get);
                string column_get = Link(column_list_get,",");
                string value_get = Link(value_list_get,",");
                sql.push_back("INSERT INTO " + table_name + "_" + to_string(frags[i].site) + " (" + column_get + ") VALUES (" + value_get + ")");
            }
        }
    }
    else {
        cout << "ERROR: INVALID FRAGMENT TYPE!" << endl;
    }
    return sql;
}
vector<string> GetsitenameInsert(string sql_statement) {
    vector<string> sitenames;
    sitenames.clear();
    string table_name;
    vector<string> column_list;
    vector<string> value_list;
    table_name = GetTableListFromInsert(sql_statement);
    column_list = GetColumnListFromInsert(sql_statement,table_name);
    value_list = GetValueListFromInsert(sql_statement);
    
    // metadata //
    map<string,Fragment> FragmentMap = GetETCDFragment();
    map<string,string> TableKeyMap = GetETCDTableKey();

    Fragment fragment = getFragFromEtcd(table_name);
    // Fragment fragment = FragmentMap[table_name];
    TraverseFragment(fragment);
    string type = fragment.fragtype;
    vector<FragDef> frags = fragment.frags;
    vector<string> value_list_get;
    vector<string> column_list_get;
    if (type == "H"){
        for (int i = 0; i < frags.size(); i++){
            string column = frags[i].column;
            vector<string> column_in_frag_list = GetList(column,",",";");
            string condition = frags[i].condition;
            vector<string> condition_list = GetList(condition, "AND",";");
            cout << "HHHHH" << endl;
            cout << "column_in_frag_list :" << endl;
            Traverse(column_in_frag_list);
            cout << "column_list :" << endl;
            Traverse(column_list);
            if (JudgeHit2(column_in_frag_list,column_list)){
                if (Judge(condition_list,column_in_frag_list,column_list,value_list)) {
                    sitenames.push_back("s" + to_string(frags[i].site));
                }
            }
        }
    }
    else if (type == "V") {
        for (int i = 0; i < frags.size(); i++) {
            string column = frags[i].column;
            vector<string> column_in_frag_list = GetList(column,",",";");
            if (JudgeHit2(column_in_frag_list, column_list)) {
                column_list_get.clear();
                value_list_get.clear();
                for (int j = 0; j < column_in_frag_list.size(); j++) {
                    int loc = GetLocateHit(column_list,column_in_frag_list[j]);
                    column_list_get.push_back(column_list[loc]);
                    value_list_get.push_back(value_list[loc]);
                }
                string column_get = Link(column_list_get,",");
                string value_get = Link(value_list_get,",");
                sitenames.push_back("s" + to_string(frags[i].site));
            }
        }
    }
    else {
        cout << "INSERT ERROR: INVALID FRAGMENT TYPE!" << endl;
    }
    return sitenames;
}
vector<string> GetPure(vector<string> column_list_get) {
    vector<string> column_list_pure;
    for (int i = 0; i < column_list_get.size(); i++) {
        column_list_pure.push_back(GetExactAfter(column_list_get[i],"."));
    }
    return column_list_pure;
}