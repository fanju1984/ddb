#include "Parse_DELETE.h"

using namespace std;

// int main() { 

//     vector<string> sitename;
//     vector<string> sql;
//     string sql_statement;

//     // sql_statement = "DELETE FROM TABLE orders;";    
//     // sitename = GetsitenameDelete(sql_statement);
//     // sql = GetsqlDelete(sql_statement);
//     // cout << sql_statement << "/" << endl;
//     // cout << "sitename>>>" << endl;
//     // Traverse(sitename);
//     // cout << "sql>>>" << endl;
//     // Traverse(sql);

//     // sql_statement = "DELETE FROM TABLE book WHERE book.copies=100 and book.id=1 and book.nation='PRC';";
//     // cout << sql_statement << "/" << endl;
//     // sitename = GetsitenameDelete(sql_statement);
//     // sql = GetsqlDelete(sql_statement);
//     // cout << "sitename>>>" << endl;
//     // Traverse(sitename);
//     // cout << "sql>>>" << endl;
//     // Traverse(sql);
//     sql_statement = "DELETE FROM TABLE publisher WHERE publisher.nation='PRC';"; //ok
//     cout << sql_statement << "/" << endl;
//     sitename = GetsitenameDelete(sql_statement);
//     sql = GetsqlDelete(sql_statement);
//     cout << "sitename>>>" << endl;
//     Traverse(sitename);
//     cout << "sql>>>" << endl;
//     Traverse(sql);

//     // sql_statement = "DELETE FROM TABLE book WHERE book.id=1;"; //ok
//     // cout << sql_statement << "/" << endl;
//     // sitename = GetsitenameDelete(sql_statement);
//     // sql = GetsqlDelete(sql_statement);
//     // cout << "sitename>>>" << endl;
//     // Traverse(sitename);
//     // cout << "sql>>>" << endl;
//     // Traverse(sql);

//     return 0;
// }
string GetSelectFromDelete(string sql_statement) {
    map<string,string> TableKeyMap = GetETCDTableKey();
    string table_name;
    string key;
    string sql_statement_select;

    table_name = GetTableList(sql_statement)[0];
    key = TableKeyMap[table_name];
    sql_statement_select = "SELECT " + key + " FROM " + GetAfter(sql_statement,"FROM");
    return sql_statement_select;
}
vector<string> GetDeleteSqlH(string sql_statement, Fragment fragment) {
    vector<string> sql;
    for (int i = 0; i < fragment.frags.size(); i++) {
        string table_name = GetTableList(sql_statement)[0];
        string deletesql = GetBefore(sql_statement,table_name) + table_name + "_" + to_string(fragment.frags[i].site) + GetAfter(sql_statement,table_name);
        cout << "deletesql :" << deletesql << "/" << endl;
        sql.push_back(deletesql);
    }
    return sql;
}
vector<string> GetDeleteSiteH(string sql_statement, Fragment fragment) {
    vector<string> sitenames;
    for (int i = 0; i < fragment.frags.size(); i++) {
        sitenames.push_back("s" + to_string(fragment.frags[i].site));
    }
    return sitenames;
}

vector<string> GetsqlDelete(string sql_statement) {
    vector<string> sql;
    sql.clear();
    string table_name;
    vector<string> column_list;
    vector<string> value_list;

    map<string,Fragment> FragmentMap = GetETCDFragment();
    map<string,string> TableKeyMap = GetETCDTableKey();
    table_name = GetTableListFromDelete(sql_statement);
    Fragment fragment = getFragFromEtcd(table_name);
    // Fragment fragment = FragmentMap[table_name];
    vector<FragDef> frags = fragment.frags;
    if (sql_statement.find("WHERE") == -1) {
        for(int i = 0; i < frags.size(); i++) {
            sql.push_back("DELETE FROM " + table_name + "_" + to_string(frags[i].site));
        }
        return sql;
    }
    
    column_list = GetColumnListFromDelete(sql_statement);
    value_list = GetValueListFromDelete(sql_statement);
    string aftertable = GetExactAfter(sql_statement,table_name);
    aftertable = GetBefore(aftertable,";");
    vector<string> aftertable_list = GetList(aftertable," and ","?");
    for (int i = 0; i < aftertable_list.size(); i++) {
        aftertable_list[i] = GetExactAfter(aftertable_list[i],".");
    }
    aftertable = Link(aftertable_list," and ");
    aftertable = " WHERE " + aftertable;

    // cout << "aftertable" << aftertable << "/" << endl;
    

    // Fragment fragment = getFragFromEtcd(table_name);
    
    string type = fragment.fragtype;
    vector<string> value_list_get;
    vector<string> column_list_get;

    // cout<< "table_name :" << table_name << "/" << endl;
    // cout << "column_list>>>" << endl;
    // Traverse(column_list);
    // cout << "value_list>>>" << endl;
    // Traverse(value_list);
    // cout << "aftertable :" << aftertable << "/" << endl;

    if (type == "H"){
        // cout << "type :" << type << "/" << endl;
        // cout << "frags.size :" << frags.size() << endl;
        for (int i = 0; i < frags.size(); i++) {
            string column = frags[i].column;
            vector<string> column_in_frag_list = GetList(column,",",";");
            string condition = frags[i].condition;
            vector<string> condition_list = GetList(condition, "AND",";");
            // cout << "column :" << column << "/" << endl;
            // cout << "column_in_frag_list>>>" << endl;
            // Traverse(column_in_frag_list);
            // cout << "column_list>>>" << endl;
            // Traverse(column_list);
            // cout << "condition :" << condition << endl;
            // cout << "condition_list>>>" << endl;
            // Traverse(condition_list);
            // cout << "value_list>>>" << endl;
            // Traverse(value_list);
            if (!JudgeHit2(column_in_frag_list,column_list)) {
                sql.push_back("DELETE FROM " + table_name + "_" + to_string(frags[i].site) + aftertable);
            }
            else {
                if (JudgeDelete(condition_list,column_in_frag_list,column_list,value_list)) {
                    sql.push_back("DELETE FROM " + table_name + "_" + to_string(frags[i].site) + aftertable);
                }
            }
            
        }
    }
    // else if (type == "V") {
    //     for (int i = 0; i < frags.size(); i++) {
    //         string column = frags[i].column;
    //         vector<string> column_in_frag_list = GetList(column,",",";");
    //         if (JudgeHit2(column_in_frag_list, column_list)) {
    //             column_list_get.clear();
    //             value_list_get.clear();
    //             for (int j = 0; j < column_in_frag_list.size(); j++) {
    //                 int loc = GetLocateHit(column_list,column_in_frag_list[j]);
    //                 column_list_get.push_back(column_list[loc]);
    //                 value_list_get.push_back(value_list[loc]);
    //             }
    //             string column_get = Link(column_list_get,",");
    //             string value_get = Link(value_list_get,",");
    //             sql.push_back("DELETE FROM " + table_name + "_" + to_string(frags[i].site) + aftertable);
    //         }
    //     }
    // }
    else {
        cout << "ERROR: INVALID FRAGMENT TYPE!" << endl;
    }
    return sql;
}

vector<string> GetsitenameDelete(string sql_statement) {
    // Fragment fragment = getFragFromEtcd(table_name);
    vector<string> sitename;
    sitename.clear();
    string table_name;
    vector<string> column_list;
    vector<string> value_list;
    map<string,Fragment> FragmentMap = GetETCDFragment();
    map<string,string> TableKeyMap = GetETCDTableKey();
    table_name = GetTableListFromDelete(sql_statement);
    column_list = GetColumnListFromDelete(sql_statement);
    value_list = GetValueListFromDelete(sql_statement);

    Fragment fragment = getFragFromEtcd(table_name);
    // Fragment fragment = FragmentMap[table_name];
    vector<FragDef> frags = fragment.frags;
    
    if (sql_statement.find("WHERE") == -1) {
        for(int i = 0; i < frags.size(); i++) {
            sitename.push_back("s" + to_string(frags[i].site));
        }
        return sitename;
    }


    vector<string> value_list_get;
    vector<string> column_list_get;

    cout<< "table_name :" << table_name << "/" << endl;
    cout << "column_list>>>" << endl;
    Traverse(column_list);
    cout << "value_list>>>" << endl;
    Traverse(value_list);
    string type = fragment.fragtype;
    cout << type << endl;
    cout << table_name << "/" << endl;
    if (type == "H"){
        cout << "type :" << type << "/" << endl;
        cout << "frags.size :" << frags.size() << endl;
        for (int i = 0; i < frags.size(); i++) {
            cout << "i = " << i << " ====" << endl;
            string column = frags[i].column;
            vector<string> column_in_frag_list = GetList(column,",",";");
            string condition = frags[i].condition;
            vector<string> condition_list = GetList(condition, "AND",";");
            // cout << "column :" << column << "/" << endl;
            // cout << "column_in_frag_list>>>" << endl;
            // Traverse(column_in_frag_list);
            // cout << "column_list>>>" << endl;
            // Traverse(column_list);
            // cout << "condition :" << condition << endl;
            // cout << "condition_list>>>" << endl;
            // Traverse(condition_list);
            // cout << "value_list>>>" << endl;
            // Traverse(value_list);
            if (!JudgeHit2(column_in_frag_list,column_list)) {
                cout << "!JudgeHit2" << endl;
                sitename.push_back("s" + to_string(frags[i].site));
            }
            else {
                if (JudgeDelete(condition_list,column_in_frag_list,column_list,value_list)) {
                    cout << "JudgeDelete" << endl;
                    sitename.push_back("s" + to_string(frags[i].site));
                }
            }            
        }
    }
    // else if (type == "V") {
    //     string select_sql_statement = "SELECT * " + GetAfter(sql_statement, "TABLE");
    //     cout << "select_sql_statement :" << select_sql_statement << "/" << endl;
    //     time_t myt = time(NULL);
    //     TREE Tree = SELECT(sql_statement,myt);
    //     exec_tree exe = Data_Select_Execute(Tree);

    // }
    else {
        cout << "ERROR: INVALID FRAGMENT TYPE!" << endl;
    }
    return sitename;
}
string GetSelectSql(string sql_statement) {
    string select_sql = "SELECT ";
    // "SELECT " + key + " FROM " + GetAfter(sql_statement, "TABLE");
    string table_name = GetBetween(sql_statement, "TABLE", " WHERE");
    map<string,string> TableKeyMap = GetETCDTableKey();
    string key = TableKeyMap[table_name];
    select_sql = select_sql + table_name + "." + key + " FROM " + table_name + " WHERE " + GetAfter(sql_statement,"WHERE");
    return select_sql;
}
string GetTableListFromDelete(string sql_statement) {
    string table_name;
    if (sql_statement.find("WHERE") != -1) {
        table_name = GetBetween(sql_statement, "TABLE"," WHERE");
    }
    else {
        table_name = GetBetween(sql_statement, "TABLE", ";");
    }
    return table_name;
}
vector<string> GetColumnListFromDelete(string sql_statement) {
    vector<string> column_list;
    string condition_name;
    vector<string> condition_list;

    condition_name = GetBetween(sql_statement,"WHERE", ";");
    condition_list = GetList(condition_name,"and","?");

    string table_name;
    if (sql_statement.find("WHERE") != -1) {
        table_name = GetBetween(sql_statement,"TABLE", " WHERE");
    }
    else {
        table_name = GetBetween(sql_statement,"TABLE",";");
    }
    // cout << "condition_list>>>" << endl;
    // Traverse(condition_list);

    for (int i = 0; i < condition_list.size(); i++) {
        if (condition_list[i].find(">=") != -1) {
            column_list.push_back(GetBefore(condition_list[i],">="));
        }
        else if (condition_list[i].find("<=") != -1) {
            column_list.push_back(GetBefore(condition_list[i],"<="));
        }
        else if (condition_list[i].find(">") != -1) {
            column_list.push_back(GetBefore(condition_list[i],">"));
        }
        else if (condition_list[i].find("<") != -1) {
            column_list.push_back(GetBefore(condition_list[i],"<"));
        }
        else if (condition_list[i].find("=") != -1) {
            column_list.push_back(GetBefore(condition_list[i],"="));
        }
    }
    for (int i = 0; i < column_list.size(); i++) {
        if (column_list[i].find(".") == -1) {
            column_list[i] = table_name + "." + column_list[i];
        }
    }
    for (int i = 0; i < column_list.size(); i++) {
        if (column_list[i].find(" ") != -1) {
            column_list[i] = GetExactAfter(column_list[i]," ");
        }
    }
    return column_list;
}
vector<string> GetValueListFromDelete(string sql_statement) {
    string value_name;
    value_name = GetAfter(sql_statement,"WHERE");
    vector<string> value_list = GetList(value_name," and ",";");
    cout << "value_list in GetValueListFromDelete>>>" << endl;
    Traverse(value_list);
    // cout << "value_list.size :" << value_list.size() << endl;
    // cout << value_list[0] << "/" << endl;
    // cout << value_list[1] << "/" << endl;
    // cout << value_list[2] << "/" << endl;
    for (int i = 0; i < value_list.size(); i++) {
        // cout << "value_list :" << value_list[i] << "/" << endl;
        if (value_list[i].find(">=")!= -1) {
            value_list[i] = GetExactAfter(value_list[i],">=");
        }
        else if (value_list[i].find("<=") != -1) {
            value_list[i] = GetExactAfter(value_list[i],"<=");
        }
        else if (value_list[i].find(">") != -1){
            value_list[i] = GetExactAfter(value_list[i], ">");
        }
        else if (value_list[i].find("<") != -1) {
            value_list[i] = GetExactAfter(value_list[i],"<");
        }
        else if (value_list[i].find("=") != -1) {
            // cout << "= in GetValueListFromDelete" << endl;
            value_list[i] = GetExactAfter(value_list[i],"=");
            // cout << value_list[i] << "/" << endl;
        }
        else {
            cout << "ERROR IN GetValueListFromDelete " << endl;
        }
        // return value_list;
    }
    // for (int i = 0; i < value_list.size(); i++) {
    //     if (value_list[i].find(" ") != -1) {
    //         value_list[i] = GetExactAfter(value_list[i]," ");
    //     }
    // }
    return value_list;
}

bool JudgeDelete(vector<string> condition_list, vector<string> condition_column_list, vector<string> insert_column_list, vector<string> insert_value_list) {
    if (condition_list.size() !=condition_column_list.size()) {
        cout << "ERROR IN Judge" << endl;
        cout << "condition_list>>>" << endl;
        Traverse(condition_list);
        cout << "condition_column_list>>>" << endl;
        Traverse(condition_column_list);
        return false;
    }

    // cout << "JudgeDelete" << endl;
    // cout << "condition_list>>>" << endl;
    // Traverse(condition_list);
    // cout << "condition_column_list>>>" << endl;
    // Traverse(condition_column_list);
    // cout << "delete_column_list>>>" << endl;
    // Traverse(insert_column_list);
    // cout << "delete_value_list>>>" << endl;
    // Traverse(insert_value_list);

    for (int i = 0; i < condition_list.size(); i++) {
        string value;
        if (JudgeHit(insert_column_list,condition_column_list[i])){ 
            int loc = GetLocateHit(insert_column_list,condition_column_list[i]);
            value = insert_value_list[loc];
            string condition_value;
            if (condition_list[i].find(">=") != -1) {
                // cout << ">=" << endl;
                condition_value = GetExactAfter(condition_list[i],">=");
                if (condition_value.find("'") == -1 && value.find("'") == -1) {
                    int condition_value_int = stoi(condition_value);
                    int value_int = stoi(value);
                    if (value_int < condition_value_int) {
                        return false;
                    }
                }
            }
            else if (condition_list[i].find("<=") != -1) {
                // cout << "<=" << endl;
                condition_value = GetExactAfter(condition_list[i],"<=");
                if (condition_value.find("'") == -1 && value.find("'") == -1) {
                    int condition_value_int = stoi(condition_value);
                    int value_int = stoi(value);
                    if (value_int > condition_value_int) {
                        return false;
                    }
                }
            }    
            else if (condition_list[i].find(">") != -1) {
                // cout << ">" << endl;
                condition_value = GetExactAfter(condition_list[i],">");
                if (condition_value.find("'") == -1 && value.find("'") == -1) {
                    int condition_value_int = stoi(condition_value);
                    int value_int = stoi(value);
                    if (value_int <= condition_value_int) {
                        return false;
                    }
                }
            }
            else if (condition_list[i].find("<") != -1) {
                // cout << "<" << endl;
                condition_value = GetExactAfter(condition_list[i],"<");
                if (condition_value.find("'") == -1 && value.find("'") == -1) {
                    int condition_value_int = stoi(condition_value);
                    int value_int = stoi(value);
                    if (value_int >= condition_value_int) {
                        return false;
                    }
                }
            }
            else if (condition_list[i].find("=") != -1) {
                // cout << "=" << endl;
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
        else {
            continue;
        }
    }
    return true;
}