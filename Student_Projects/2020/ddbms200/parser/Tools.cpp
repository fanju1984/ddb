#include "Tools.h"
using namespace std;

// // NEED TO DELETE //
// struct ColumnDef {
// 	string name;
// 	string type;
// 	bool null=false;
// 	bool key=false;
// 	string desc;
// };
// // table 
// struct GDD {
// 	string name;
// 	vector<ColumnDef> cols;
// };
// // NEED TO DELETE // 

string GetBetween(string sql_statement, string start, string end) {
    if (sql_statement.find(start) == -1 || sql_statement.find(end) == -1) {
        return "";
    }
    else {
        int start_loc = sql_statement.find(start) + start.size() + 1;
        int end_loc = sql_statement.find(end);
        return sql_statement.substr(start_loc, end_loc-start_loc);
    }
}
string GetAfter(string sql_statement, string start) {
    if (sql_statement.find(start) == -1) {
        return "";
    }
    else {
        int start_loc = sql_statement.find(start) + start.size() + 1;
        int end_loc = sql_statement.size();
        return sql_statement.substr(start_loc, end_loc-start_loc);
    }
}
string GetExactAfter(string sql_statement, string start) {
    if (sql_statement.find(start) == -1) {
        return "";
    }
    else {
        int start_loc = sql_statement.find(start) + start.size();
        int end_loc = sql_statement.size();
        return sql_statement.substr(start_loc, end_loc-start_loc);
    }
}
string GetBefore(string sql_statement, string end) {
    if (sql_statement.find(end) == -1) {
        return "";
    }
    else {
        int end_loc = sql_statement.find(end);
        return sql_statement.substr(0,end_loc);
    }    
}
vector<string> GetList(string Line, string split, string stop) {
    vector <string> TableList;
    int loc_start = 0;
    int loc_end;
    int size = Line.size();
    int cnt = 0;
    while (size > 0) {
        loc_end = Line.find(split);
        int loc_table_end = loc_end + split.size();
        if (loc_end == -1) {
            string item = Line;
            item = item.substr(0,item.find(stop)-stop.size()+1);
            TableList.push_back(item);
            break;
        }
        else {
            string item = Line.substr(0,loc_end);
            TableList.push_back(item);
            size = size - loc_table_end;
            Line = Line.substr(loc_table_end,size);
        }       
    }
    return TableList;
}
void Traverse(vector<string> input) {
    int size = input.size();
    for (int i = 0; i < size; i++) {
        cout << input[i] << "/" << endl;
    }
}
string GetTableName(string sql_statement) {
    if (sql_statement.find("WHERE") == -1) {
        return GetBetween(sql_statement, "FROM", ";");
    }
    else {
        return GetBetween(sql_statement, "FROM", " WHERE");
    }
}
vector<string> GetTableList(string sql_statement) {
    return GetList(GetTableName(sql_statement),","," ");
}
string GetSelectColumnName(string sql_statement) {
    return GetBetween(sql_statement, "SELECT", "FROM");
}
vector<string> GetSelectColumnList(string sql_statement) {
    return GetList(GetSelectColumnName(sql_statement),","," ");
}
string GetCondition(string sql_statement) {
    // return GetAfter(sql_statement,"WHERE");
    return GetBetween(sql_statement,"WHERE",";");
}
vector<string> GetConditionList(string sql_statement) {
    string condition = GetCondition(sql_statement);
    if (condition != " " && condition != "") {
        return GetList(condition," and ",";");
    }
    else {
        vector<string> condition_list;
        condition_list.clear();
        return condition_list;
    }
}
string GetTableFromColumn(string column, vector<string> TableList) {
    int size = TableList.size();
    for (int i = 0; i < size; i++) {
        if (column.find(TableList[i]) != -1){
            return TableList[i];
        }
    }
    return "GetTableFromColumn ERROR!";
}
vector<string> GetColumnFromCondition(string condition, vector<string> TableList) {
    vector<string> items;
    vector<string> column_list;
    if (condition.find('=') != -1) {
        items = GetList(condition,"="," ");
    }
    else if (condition.find('>') != -1) {
        items = GetList(condition,">"," ");
    }
    else if (condition.find('<') != -1) {
        items = GetList(condition,"<"," ");
    }
    else if (condition.find(">=") != -1) {
        items = GetList(condition, ">="," ");
    }
    else if (condition.find("<=") != -1) {
        items = GetList(condition, "<=", " ");
    }
    else {
        column_list.clear();
        return column_list;
    }
    for (int i = 0; i < items.size(); i++) {
        if(GetTableFromColumn(items[i], TableList) != "GetTableFromColumn ERROR!") {
            column_list.push_back(items[i]);
        }
    }
    return column_list;
}
vector<string> GetColumnListFromConditionList(vector<string> ConditionList, vector<string> TableList) {
    vector<string> ColumnList;
    for(int i = 0; i < ConditionList.size(); i++) {
        vector<string> tmp = GetColumnFromCondition(ConditionList[i], TableList);
        ColumnList.insert(ColumnList.end(), tmp.begin(), tmp.end());
    }
    return ColumnList;
}
vector<string> GetAllColumnList(string sql_statement) {
    vector<string> select_column_list = GetSelectColumnList(sql_statement);
    vector<string> condition_list = GetConditionList(sql_statement);
    vector<string> column_list = GetColumnListFromConditionList(condition_list, GetTableList(sql_statement));
    // cout << "select_column_list IN GetAllColumnList >>> " << endl;
    // Traverse(select_column_list);
    // cout << "condition_list IN GetAllColumnList >>> " << endl;
    // Traverse(condition_list);
    // cout << "column_list IN GetAllColumnList >>> " << endl;
    // Traverse(column_list);
    column_list.insert(column_list.end(),select_column_list.begin(),select_column_list.end());
    return column_list;
}
vector<string> GetAllDifferentColumnList(string sql_statement) {
    vector<string> column_list = GetAllColumnList(sql_statement);
    vector<string> different_list;
    different_list.clear();
    for (int i = 0; i < column_list.size(); i++) {
        if (!JudgeHit(different_list,column_list[i])) {
            different_list.push_back(column_list[i]);
        }
    }
    return different_list;
}
vector<string> GetAllDifferentTreeNodeColumnList(string sql_statement, string table_1, string treenode1, string table_2, string treenode2) {
    vector<string> different_list;
    different_list = GetAllDifferentColumnList(sql_statement);
    vector<string> differenttreenodecollumnlist;
    cout << "table_1 :" << table_1 << endl;
    cout << "table_2 :" << table_2 << endl;
    for (int i = 0; i < different_list.size(); i++) {
        if (GetBefore(different_list[i],".") == table_1) {
            differenttreenodecollumnlist.push_back(treenode1 + "." + GetExactAfter(different_list[i],"."));
        }
        else if (GetBefore(different_list[i],".") == table_2) {
            differenttreenodecollumnlist.push_back(treenode2 + "." + GetExactAfter(different_list[i],"."));
        }
    }
    return differenttreenodecollumnlist;
}

// vector<string> GetCollumnsSelect(vector<string> table_list, vector<TCC> TCCList) {
//     vector<string> collumn_list;
//     for (int i = 0; i < table_list.size(); i++) {
//         for (int j = 0; j < TCCList.size(); j++) {
//             if (TCCList[j].table_name == table_list[i]) {
//                 for (int k = 0; k < TCCList[j].column_list.size(); k++ ){
//                     string item = GetExactAfter(TCCList[j].column_list[k],".");
//                     collumn_list.push_back(item);
//                 }
                
//             }
//         }
//     }
//     collumn_list = GetCleanList(collumn_list);
//     return collumn_list;
// }
vector<string> GetAllTreeNodeCollumnList(string sql_statement, string table_1, string treenode1, string table_2, string treenode2) {
    vector<string> treenodecollumnlist;
    vector<string> allcollumnlist = GetAllColumnList(sql_statement);
    for (int i = 0; i < allcollumnlist.size(); i++) {
        if (GetBefore(allcollumnlist[i],".") == table_1) {
            treenodecollumnlist.push_back(treenode1 + "." + GetExactAfter(allcollumnlist[i],"."));
        }
        else if (GetBefore(allcollumnlist[i],".") == table_2) {
            treenodecollumnlist.push_back(treenode2 + "." + GetExactAfter(allcollumnlist[i],"."));
        }
    } 
    return treenodecollumnlist;
}
vector<string> GetAllData(string sql_statement) {
    vector<string> ConditionList = GetConditionList(sql_statement);
    vector<string> DataList;
    for (int i = 0; i < ConditionList.size(); i++) {
        string condition = ConditionList[i];
        if(GetColumnFromCondition(condition, GetTableList(sql_statement)).size() == 2) {
            cout << " THE JOIN SENTENCE " << endl;
        }
        else if(GetColumnFromCondition(condition, GetTableList(sql_statement)).size() == 1) {
            cout << " THE SIGMA SENTENCE " << endl;
            
        }
        else {
            cout << " CONDITION PARSING ERROR " << endl;
        }
    }
    return DataList;
}
int GetTCLoc(string table, string column) {
    return 0;
}
string GetPureColumnFromColumn(string column) {
    int pure_column_loc = column.find(".")+1;
    string pure_column = column.substr(pure_column_loc, column.size() - pure_column_loc);
    return pure_column;
}
string Link(vector<string> input, string devide) {
    string output = input[0];
    for (int i = 1; i < input.size(); i++) {
        output += devide + input[i];
    }
    return output;
}
void Traversefrags(vector<FragDef> frags) {
    for (int i = 0; i < frags.size(); i++) {
        cout << "frags[" << to_string(i) << "] >>>" << endl;
        cout << frags[i].column << "/" << endl;
        cout << frags[i].condition << "/" << endl;
        cout << frags[i].id << "/" << endl;
        cout << frags[i].site << "/" <<endl;
        cout << frags[i].size << "/" << endl;
    }
}
void TraverseFragment(Fragment fragment) {
    cout << fragment.name << endl;
    cout << fragment.fragtype << endl;
    cout << fragment.fragnum << endl;
    Traversefrags(fragment.frags);
}
map<string,Fragment> GetETCDFragment() {
    map<string,Fragment> FragmentMap;
    Fragment fragment_tmp;
    vector<FragDef> frags_tmp;
    FragDef frag_tmp;

    fragment_tmp.name = "customer";
    fragment_tmp.fragtype = "V";
    fragment_tmp.fragnum = 2;
    fragment_tmp.frags.clear();
    frag_tmp.id = 1;
    frag_tmp.site = 1;
    frag_tmp.size = 0;
    frag_tmp.condition = "customer.idANDcustomer.name";
    frag_tmp.column = "customer.id,customer.name";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 2;
    frag_tmp.site = 2;
    frag_tmp.size = 0;
    frag_tmp.condition = "customer.idANDcustomer.rank";
    frag_tmp.column = "customer.id,customer.rank";
    fragment_tmp.frags.push_back(frag_tmp);
    FragmentMap["customer"] = fragment_tmp;

    fragment_tmp.name = "orders";
    fragment_tmp.fragtype = "H";
    fragment_tmp.fragnum = 4;
    fragment_tmp.frags.clear();
    frag_tmp.id = 1;
    frag_tmp.site = 1;
    frag_tmp.size = 0;
    frag_tmp.condition = "orders.customer_id<307000ANDorders.customer_id<215000";
    frag_tmp.column = "orders.customer_id,orders.customer_id";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 2;
    frag_tmp.site = 2;
    frag_tmp.size = 0;
    frag_tmp.condition = "orders.customer_id<307000ANDorders.book_id>=215000";
    frag_tmp.column = "orders.customer_id,orders.customer_id";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 3;
    frag_tmp.site = 3;
    frag_tmp.size = 0;
    frag_tmp.condition = "orders.customer_id>=307000ANDorders.book_id<215000";
    frag_tmp.column = "orders.customer_id,orders.customer_id";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 4;
    frag_tmp.site = 4;
    frag_tmp.size = 0;
    frag_tmp.condition = "orders.cusomer_id>=307000ANDorders.book_id>=215000";
    frag_tmp.column = "orders.customer_id,orders.customer_id";
    fragment_tmp.frags.push_back(frag_tmp);
    FragmentMap["orders"] = fragment_tmp;

    fragment_tmp.name = "publisher";
    fragment_tmp.fragtype = "H";
    fragment_tmp.fragnum = 4;
    fragment_tmp.frags.clear();
    frag_tmp.id = 1;
    frag_tmp.site = 1;
    frag_tmp.size = 0;
    frag_tmp.condition = "publisher.id<104000ANDpublisher.nation='PRC'";
    frag_tmp.column = "publisher.id,publisher.nation";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 2;
    frag_tmp.site = 2;
    frag_tmp.size = 0;
    frag_tmp.condition = "publisher.id<104000ANDpublisher.nation='USA'";
    frag_tmp.column = "publisher.id,publisher.nation";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 3;
    frag_tmp.site = 3;
    frag_tmp.size = 0;
    frag_tmp.condition = "publisher.id>=104000ANDpublisher.nation='PRC'";
    frag_tmp.column = "publisher.id,publisher.nation";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 4;
    frag_tmp.site = 4;
    frag_tmp.size = 0;
    frag_tmp.condition = "publisher.id>=104000ANDpublisher.nation='USA'";
    frag_tmp.column = "publisher.id,publisher.nation";
    fragment_tmp.frags.push_back(frag_tmp);
    FragmentMap["publisher"] = fragment_tmp;

    fragment_tmp.name = "book";
    fragment_tmp.fragtype = "H";
    fragment_tmp.fragnum = 4;
    fragment_tmp.frags.clear();
    frag_tmp.id = 1;
    frag_tmp.site = 1;
    frag_tmp.size = 0;
    frag_tmp.condition = "book.id<205000";
    frag_tmp.column = "book.id";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 2;
    frag_tmp.site = 2;
    frag_tmp.size = 0;
    frag_tmp.condition = "book.id>=205000ANDbook.id<210000";
    frag_tmp.column = "book.id,book.id";
    fragment_tmp.frags.push_back(frag_tmp);
    frag_tmp.id = 3;
    frag_tmp.site = 3;
    frag_tmp.size = 0;
    frag_tmp.condition = "book.id>=210000";
    frag_tmp.column = "book.id";
    fragment_tmp.frags.push_back(frag_tmp);
    FragmentMap["book"] = fragment_tmp;
    return FragmentMap; 
}
map<string,string> GetETCDTableKey() {
    map<string,string> TableKeyMap;
    TableKeyMap["customer"] = "id";
    TableKeyMap["orders"] = "customer_id";
    TableKeyMap["publisher"] = "id";
    TableKeyMap["book"] = "id";
    return TableKeyMap;
}
map<string,GDD> GetETCDGDD() {
    map<string,GDD> GDDMap;
    GDD gdd_tmp;
    ColumnDef columndef;
    vector<ColumnDef> columndef_list;

    gdd_tmp.name = "customer";
    columndef_list.clear();
    
    columndef.name="id";
    columndef.type="int(24)";
    columndef.key=true;
    columndef_list.push_back(columndef);

    columndef.name="name";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    columndef.name="rank";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);
    gdd_tmp.cols=columndef_list;
    GDDMap["customer"] = gdd_tmp;

    gdd_tmp.name = "orders";
    columndef_list.clear();

    columndef.name="customer_id";
    columndef.type="int(24)";
    columndef.key=true;
    columndef_list.push_back(columndef);

    columndef.name="book_id";
    columndef.type="int(24)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    columndef.name="quantity";
    columndef.type="int(24)";
    columndef.key=false;
    columndef_list.push_back(columndef);
    gdd_tmp.cols = columndef_list;
    GDDMap["orders"] = gdd_tmp;

    gdd_tmp.name="publisher";
    columndef_list.clear();

    columndef.name="id";
    columndef.type="char(100)";
    columndef.key=true;
    columndef_list.push_back(columndef);

    columndef.name="name";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);
    
    columndef.name="nation";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    gdd_tmp.cols=columndef_list;
    GDDMap["publisher"]=gdd_tmp;

    gdd_tmp.name="book";
    columndef_list.clear();

    columndef.name="id";
    columndef.type="int(24)";
    columndef.key=true;
    columndef_list.push_back(columndef);

    columndef.name="title";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    columndef.name="authors";
    columndef.type="char(100)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    columndef.name="publisher_id";
    columndef.type="int(24)";
    columndef.key=false;
    columndef_list.push_back(columndef);

    columndef.name="copies";
    columndef.type="int(24)";
    columndef.key=false;
    columndef_list.push_back(columndef);
    gdd_tmp.cols=columndef_list;
    GDDMap["book"] = gdd_tmp;
    return GDDMap;
}
void TraverseGDD(GDD gdd){
    cout << gdd.name << "/" << endl;
    TraverseGDDCol(gdd.cols);
}
void TraverseGDDCol(vector<ColumnDef> cols) {
    for (int i = 0; i < cols.size(); i++) {
        cout << cols[i].name << "/" << endl;
        cout << cols[i].type << "/" << endl;
    }
}
string GetTableListFromInsert(string sql_statement) {
    string table_name = GetBetween(sql_statement, "TABLE"," (");
    return table_name;
}
vector<string> GetColumnListFromInsert(string sql_statement,string table_name) {
    string column_name;
    column_name = GetBetween(sql_statement,table_name, " VALUES");
    column_name = GetBetween(column_name,"("," )");
    vector<string> column_list = GetList(column_name,",",";");
    for (int i = 0; i < column_list.size(); i++) {
        column_list[i] = table_name + "." +column_list[i]; 
    }
    return column_list;
}
vector<string> GetValueListFromInsert(string sql_statement) {
    string value_name;
    value_name = GetBetween(sql_statement, "VALUES",";");
    value_name = GetBetween(value_name,"("," )");
    vector<string> value_list = GetList(value_name,",",";");
    return value_list;
}
bool JudgeHit(vector<string> BigList, string oneitem) {
    for (int i = 0; i < BigList.size(); i++) {
        if (oneitem == BigList[i]) {
            return true;
        }
    }
    return false;
}
bool JudgeHit2 (vector<string> FirstList, vector<string> SecondList) {
    for (int i = 0; i < FirstList.size(); i++) {
        for (int j = 0; j < SecondList.size(); j++) {
            if (FirstList[i] == SecondList[j]) {
                return true;
            }
        }
    }
    return false;
}
int GetLocateHit(vector<string> BigList, string oneitem) {
    if (JudgeHit(BigList,oneitem)) {
        for (int i = 0; i < BigList.size(); i++) {
            if (oneitem == BigList[i]) {
                return i;
            }
        }
    }
    cout << "CANT HIT" << endl;
    return 0;
}
bool Judge(vector<string> condition_list, vector<string> condition_column_list, vector<string> insert_column_list, vector<string> insert_value_list) {
    if (condition_list.size() !=condition_column_list.size()) {
        Traverse(condition_list);
        Traverse(condition_column_list);
        cout << "ERROR IN Judge" << endl;
        return false;
    }
    // cout << "condition_list>>>" << endl;
    // Traverse(condition_list);
    // cout << "delete_column_list>>" << endl;
    // Traverse(insert_column_list);

    cout << "condition_list>>>" << endl;
    Traverse(condition_list);
    cout << "condition_column_list>>>" << endl;
    Traverse(condition_column_list);
    cout << "insert_column_list>>>" << endl;
    Traverse(insert_column_list);
    cout << "insert_value_list>>>" << endl;
    Traverse(insert_value_list); 

    for (int i = 0; i < condition_list.size(); i++) {
        string value;
        cout << i << "'s condition" << endl; 
        if (JudgeHit(insert_column_list,condition_column_list[i])){ 
            int loc = GetLocateHit(insert_column_list,condition_column_list[i]);
            value = insert_value_list[loc];
            cout << "value :" << value << endl;
            cout << "loc :" << loc << endl; 
        }

        // condition
        string condition_value;
        if (condition_list[i].find(">=") != -1) {
            cout << ">=" << endl;
            condition_value = GetExactAfter(condition_list[i],">=");
            if (condition_value.find("'") == -1 && value.find("'") == -1) {
                int condition_value_int = stoi(condition_value);
                int value_int = stoi(value);
                if (value_int < condition_value_int) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (condition_list[i].find("<=") != -1) {
            cout << "<=" << endl;
            condition_value = GetExactAfter(condition_list[i],"<=");
            if (condition_value.find("'") == -1 && value.find("'") == -1) {
                int condition_value_int = stoi(condition_value);
                int value_int = stoi(value);
                if (value_int > condition_value_int) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        
        else if (condition_list[i].find(">") != -1) {
            cout << ">" << endl;
            condition_value = GetExactAfter(condition_list[i],">");
            if (condition_value.find("'") == -1 && value.find("'") == -1) {
                int condition_value_int = stoi(condition_value);
                int value_int = stoi(value);
                if (value_int <= condition_value_int) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (condition_list[i].find("<") != -1) {
            cout << "<" << endl;
            condition_value = GetExactAfter(condition_list[i],"<");
            if (condition_value.find("'") == -1 && value.find("'") == -1) {
                int condition_value_int = stoi(condition_value);
                int value_int = stoi(value);
                cout << "condition_value_int :" << condition_value_int << endl;
                cout << "value_int :" << value_int << endl;
                if (value_int >= condition_value_int) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
        else if (condition_list[i].find("=") != -1) {
            cout << "=" << endl;
            condition_value = GetExactAfter(condition_list[i],"=");
            if (condition_value != value) {
                return false;
            }
        }
        
        else {
            cout << "ERROR IN Judge: condition INVALID" << endl;
            return false;
        }
    }
    return true;
}
string GetValue(vector<string> column_in_frag_list, vector<string> value_list, vector<string> column_list) {
    string value = "";
    vector<string> value_list_get;
    if (column_list.size() != value_list.size()) {
        cout << "ERROR IN GetValue " << endl;
        return value;
    }
    for (int i = 0; i < column_list.size(); i++) {
        int loc = GetLocateHit(column_in_frag_list,column_list[i]);
        value_list_get.push_back(value_list[loc]);
    }
    value = Link(value_list_get,",");
    return value;
}
string GetInsetItem(vector<string> column_list, vector<string> value_list) {
    string sql_statement = "(";
    if (column_list.size() != value_list.size()) {
        cout << "ERROR IN GetInsertItem" << endl;
        return sql_statement;
    }
    for (int i = 0; i < column_list.size(); i++) {
        column_list[i] = GetExactAfter(column_list[i],".");
    }
    sql_statement += Link(column_list,",");
    sql_statement += ") VALUES (";
    sql_statement += Link(value_list,",");
    sql_statement += ")";
    return sql_statement;
}

void trim(string &s) {
    if( !s.empty()) {
        s.erase(0,s.find_first_not_of(" "));
        s.erase(s.find_last_not_of(" ") + 1);
    }
 }

string Getall_collumn(string table_1,string treenode1,string table_2,string treenode2) {
    string all_collumn;
    vector<string> collumn_list1;
    vector<string> collumn_list2;
    collumn_list1 = GetCollumnListOfTable(table_1);
    for (int i = 0; i < collumn_list1.size(); i++) {
        collumn_list1[i] = treenode1 + "." + collumn_list1[i];
    }
    collumn_list2 = GetCollumnListOfTable(table_2);
    for (int i = 0; i < collumn_list2.size(); i++) {
        collumn_list2[i] = treenode2 + "." + collumn_list2[i];
    }
    all_collumn = Link(collumn_list1,",")+",";
    all_collumn += Link(collumn_list2,",");
    return all_collumn;
}
vector<string> GetCollumnListOfTable(string table) {
    vector<string> collumn_list;
    map<string,GDD> GDDMap = GetETCDGDD();
    GDD gdd = GDDMap[table];
    TraverseGDD(gdd);
    cout << "gdd.cols.size :" << gdd.cols.size() << endl;
    for (int i = 0; i < gdd.cols.size(); i++) {
        cout << gdd.cols[i].name << "/" << endl;
        collumn_list.push_back(gdd.cols[i].name);
    }
    return collumn_list;
}
string GetTreeNode(int treeid,int nodeid) {
    string treenode;
    treenode = "tree_" + to_string(treeid) + "_node_" + to_string(nodeid);
    return treenode;
}
vector<string> GetAllDifferentCollumnListOfTable(string sql_statement, string table_name) {
    vector<string> differentcollumn_list;
    vector<string> table_collumn_list;
    map<string,GDD> GDDMap = GetETCDGDD();
    differentcollumn_list = GetAllDifferentColumnList(sql_statement);
    GDD gdd_tmp = GDDMap[table_name];
    for (int i = 0; i < differentcollumn_list.size(); i++) {
        if (GetBefore(differentcollumn_list[i],".") == table_name) {
            table_collumn_list.push_back(GetExactAfter(differentcollumn_list[i],"."));
        }
    }
    return table_collumn_list;
}
vector<string> GetCleanList(vector<string> input) {
    vector<string> output;
    output.clear();
    vector<int> locates;
    for (int i = 0; i < input.size(); i++) {
        if (!JudgeHit(output, input[i])) {
            output.push_back(input[i]);
        }
        else {
            int locate = GetLocateHit(output, input[i]);
            output.erase(output.begin() + locate );
        }
    }
    return output;
}
