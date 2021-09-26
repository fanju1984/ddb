#include "Parse_INIT.h"

using namespace std;

// int main() {
//     // Create table
//     string sql_statement;
//     vector<string> sql_statement_list;
//     sql_statement = "CREATE TABLE Pulisher[ id int(24) key,name char(100),nation char(100)];"; 
//     sql_statement_list.push_back(sql_statement);
//     sql_statement = "CREATE TABLE Customer[ id int(24) key,name char(100),rank char(100)];";
//     sql_statement_list.push_back(sql_statement);
//     sql_statement = "CREATE TABLE Book[ id int(24) key,title char(100),authors char(100),publisher_id int(24),copies int(24)];";
//     sql_statement_list.push_back(sql_statement);
//     sql_statement = "CREATE TABLE Orders[ customer_id int(24) key,Book_id int(24) key,quantity int(24)];";
//     sql_statement_list.push_back(sql_statement);
//     GDD gdd_tmp;
//     for (int i = 0; i < sql_statement_list.size(); i++) {
//         gdd_tmp = InitGetGDDCreateTable(sql_statement_list[i]);
//         // TraverseGDD(gdd_tmp);
//         // saveTableToEtcd(gdd_tmp);
//     }
    
//     // Create Fragment 
//     vector<string> sql_statement_list2;
//     sql_statement = "CREATE FRAGMENTATION Publisher ( H,1 Publisher.id<104000ANDPublisher.nation='PRC' 1,2 Publisher.id<104000ANDPublisher.nation='PRC' 2,3 Publisher.id>=104000ANDPublisher.nation='USA' 3,4 Publisher.id>=104000ANDPublisher.nation='USA' 4 );";
//     sql_statement_list2.push_back(sql_statement);
//     sql_statement = "CREATE FRAGMENTATION Book ( H,1 Book.id<205000 1,2 Book.id>=205000ANDid<210000 2,3 Book.id>=210000 3);";
//     sql_statement_list2.push_back(sql_statement);
//     sql_statement = "CREATE FRAGMENTATION Customer ( V,1 Customer.idANDCustomer.name 1,2 Customer.idANDCusomer.rank 2);";
//     sql_statement_list2.push_back(sql_statement);
//     sql_statement = "CREATE FRAGMENTATION Orders ( H,1 Orders.customer_id<307000ANDOrders.customer_id<215000 1,2 Orders.customer_id<307000ANDOrders.book_id>=215000 2,3 Orders.customer_id>=307000ANDOrders.book_id<215000 3,4 Orders.cusomer_id>=307000ANDOrders.book_id>=215000 4);";
//     sql_statement_list2.push_back(sql_statement);
//     Fragment fragment_tmp;
//     for (int i = 0; i < sql_statement_list2.size(); i++) {
//         cout << i << " =="<< endl;
//         fragment_tmp = InitGetFragmentCreateFragment(sql_statement_list2[i]);
//         // saveFragToEtcd(fragment_tmp);
//     }
//     return 0;
// }

// Create table
string InitGetTableCreateTable(string sql_statement) {
    return GetBetween(sql_statement, "CREATE TABLE", "[");
}
vector<ColumnDef> InitGetColumnsCreateTable(string sql_statement) {
    string column_line = GetBetween(sql_statement, "[","]");
    vector<string> column_list = GetList(column_line,",","]");
    vector<ColumnDef> cols;
    for (int i = 0; i < column_list.size(); i++ ) {
        string column_tmp = column_list[i];
        // cout << "== " << column_tmp << " ==" << endl;
        vector<string> column_tmp_list = GetList(column_tmp, " ", "]");
        ColumnDef cols_tmp;
        cols_tmp.name = column_tmp_list[0];
        cols_tmp.type = column_tmp_list[1];
        if (column_tmp_list.size() >= 3 && column_tmp_list[2] == "key") {
            cols_tmp.key = true;
        }
        cols.push_back(cols_tmp);
    }
    return cols;
}
GDD InitGetGDDCreateTable(string sql_statement) {
    GDD gdd;
    gdd.name = InitGetTableCreateTable(sql_statement);
    gdd.cols = InitGetColumnsCreateTable(sql_statement);
    return gdd;
}
// Create Fragment
string InitGetTableCreateFragmentation(string sql_statement) {
    return GetBetween(sql_statement, "CREATE FRAGMENTATION", "(");
}
string InitGetFragmentTypeCreateFragmentation(string sql_statement) {
    string fragment_line = GetBetween(sql_statement, "(", ")");
    vector<string> fragment_list = GetList(fragment_line,",",")");
    return fragment_list[0];
}
vector<FragDef> InitGetFragDefCreateFragmentation(string sql_statement) {
    string fragment_line = GetBetween(sql_statement, "(", ")");
    vector<string> fragment_list = GetList(fragment_line,",",")");
    vector<FragDef> frags;
    for (int i = 1; i < fragment_list.size(); i++) {
        string fragement_tmp = fragment_list[i];
        vector<string> fragment_tmp_list = GetList(fragement_tmp, " ", ")");
        FragDef frags_tmp;        
        frags_tmp.condition = fragment_tmp_list[1];
        frags_tmp.id = stoi(fragment_tmp_list[0]);
        frags_tmp.site = stoi(fragment_tmp_list[2]);
        frags_tmp.size = 0;
        frags_tmp.column = InitGetFragsColumn(frags_tmp.condition);
        frags.push_back(frags_tmp);
    }
    return frags;
}
Fragment InitGetFragmentCreateFragment(string sql_statement) {
    Fragment fragment;
    fragment.fragtype = InitGetFragmentTypeCreateFragmentation(sql_statement);
    cout << "fragtype :"<<fragment.fragtype << "end" <<endl;
    fragment.name = InitGetTableCreateFragmentation(sql_statement);
    cout << "name " << fragment.name <<"end"<< endl;
    fragment.frags = InitGetFragDefCreateFragmentation(sql_statement); // Segmentation fault (core dumped)
    cout << "fragment.frags" << endl;
    Traversefrags(fragment.frags);
    fragment.fragnum = fragment.frags.size();
    cout << "fragment.fragnum" << fragment.fragnum << endl; 
    return fragment;
}
int InitGetFragNumCreateFragmentation(string sql_statement) {
    string fragment_line = GetBetween(sql_statement, "(", ")");
    vector<string> fragment_list = GetList(fragment_line,",",")");
    return fragment_list.size()-1;
}
string InitGetFragsColumn(string condition) {
    string column = "";
    vector<string> condition_list = GetList(condition,"AND",";");
    // cout << "condition_list IN InitGetFragsColumn :" << endl;
    // Traverse(condition_list);
    column += InitGetColumnFromCondition(condition_list[0]);
    for (int i = 1; i < condition_list.size(); i++) {
        column += "," + InitGetColumnFromCondition(condition_list[i]);
    }
    return column;
}
string InitGetColumnFromCondition(string condition) {
    string column;
    if (condition.find(">=") != -1) {
        column = GetBefore(condition,">=");
    }
    else if (condition.find("<=") != -1) {
        column = GetBefore(condition,"<=");
    }
    else if (condition.find("=") != -1) {
        column = GetBefore(condition,"=");
    }// 将>= <= 调整到 = 前面去会避免之前的错误 Q1: Init Fragment中column中出现condition中的非column内容
    else if (condition.find(">") != -1) {
        column = GetBefore(condition,">");
    }
    else if (condition.find("<") != -1) {
        column = GetBefore(condition,"<");
    }
    else {
        column = condition;
    }   
    return column;
}