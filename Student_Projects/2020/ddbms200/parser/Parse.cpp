#include "Parse_INIT.h"
#include "Parse_LOAD.h"
#include "Parse_SELECT.h"
#include "Parse_INSERT.h"
#include "Parse_DELETE.h"
#include "Tools.h"
using namespace std;

int main() {

    string sql_statement;
    // in load function
    vector<string> sql_statements;
    string create_sql_yq;
    string load_sql_yq;
    string main_name;
    vector<string> sitenames;
    vector<string> sqls;
    vector<string> table_names;
    // in load function end

    // Fragment fragment;
    // sql_statement = "CREATE FRAGMENTATION publisher( H,1 publisher.id<104000ANDpublisher.nation='PRC' 1,2 publisher.id<104000ANDpublisher.nation='PRC' 2,3 publisher.id>=104000ANDpublisher.nation='USA' 3,4 publisher.id>=104000ANDpublisher.nation='USA' 4 );";
    // fragment = InitGetFragmentCreateFragment(sql_statement);
    // saveFragToEtcd(fragment);
    // sql_statement = "CREATE FRAGMENTATION book( H,1 book.id<205000 1,2 book.id>=205000ANDid<210000 2,3 book.id>=210000 3);";
    // fragment = InitGetFragmentCreateFragment(sql_statement);
    // saveFragToEtcd(fragment);
    // sql_statement = "CREATE FRAGMENTATION customer( V,1 customer.idANDcustomer.name 1,2 customer.idANDcusomer.rank 2);";
    // fragment = InitGetFragmentCreateFragment(sql_statement);
    // saveFragToEtcd(fragment);
    // sql_statement = "CREATE FRAGMENTATION orders( H,1 orders.customer_id<307000ANDorders.customer_id<215000 1,2 orders.customer_id<307000ANDorders.book_id>=215000 2,3 orders.customer_id>=307000ANDorders.book_id<215000 3,4 orders.customer_id>=307000ANDorders.book_id>=215000 4);";
    // fragment = InitGetFragmentCreateFragment(sql_statement);
    // saveFragToEtcd(fragment);

    // sql_statement = "SELECT customer.name,book.title,publisher.name,orders.quantity FROM customer,book,publisher,orders WHERE customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and customer.id>308000 and book.copies>100 and orders.quantity>1 and publisher.nation='PRC';";
    // sql_statement = "SELECT * FROM customer;";
    // vector<TCC> TCCList = GetTCCListTest(sql_statement);
    // TraverseTCCList(TCCList);
    // TREE Tree = SELECT(sql_statement,0);
    // TraverseTree(Tree.Nodes);
    // Data_Select_Execute(Tree);
    int treeid = 0;
    while (true) {
        sql_statement = "";
        cout << ">>>";
        getline(cin,sql_statement);
        if (sql_statement.find("LOCAL") != -1) {
            // cout << "LOCAL" << endl;
            if (sql_statement.find("CREATE TABLE") != -1) {
                create_sql_yq = GetLocalCreate(sql_statement);
            }
            else if (sql_statement.find("LOAD ALLDATA") != -1) {
                load_sql_yq = GetLoadSql(sql_statement);
                main_name = GetTableFromLocalLoad(sql_statement);
            }
            else if (sql_statement.find("LOAD LOCALDATA") != -1) {
                sql_statements.push_back(sql_statement);
            }
            else if (sql_statement.find("LOCAL FINISH") != -1) {
                sitenames = GetSiteNames(sql_statements);
                sqls = GetSqls(sql_statements);
                table_names = GetTableNames(sql_statements);

                // TEST //
                cout << "create_sql_yq :" << create_sql_yq << "/" << endl;
                cout << "load_sql_yq :" << load_sql_yq << "/" << endl;
                cout << "main_name :" << main_name << "/" << endl;
                cout << "sitenames>>>" << endl;
                Traverse(sitenames);
                cout << "sqls>>>" << endl;
                Traverse(sqls);
                cout << "table_names>>>" << endl;
                Traverse(table_names);
                // TEST END //

                string result = Data_Load_Execute(create_sql_yq,load_sql_yq,main_name,sitenames,sqls,table_names);
                cout << result << endl;
                sitenames.clear();
                sqls.clear();
                table_names.clear();
                sql_statements.clear();
            }
            else {
                cout << "INVALID LOAD COMMENT" << endl;
            }
            continue;
        }
        else if (sql_statement.find("CLEAR") != -1) {
            vector<string> sql;
            vector<string> sitename;
            sql.push_back("DROP TABLE publisher");
            sql.push_back("DROP TABLE orders");
            sql.push_back("DROP TABLE book");
            sql.push_back("DROP TABLE customer");
            sitename.push_back("s1");
            sitename.push_back("s1");
            sitename.push_back("s1");
            sitename.push_back("s1");
            Traverse(sql);
            Traverse(sitename);
            cout << Data_Insert_Delete_Execute(sitename,sql);
        }
        else if (sql_statement.find("CREATE TABLE") != -1) {
            // cout << "CREATE TABLE" << endl;
            GDD gdd = InitGetGDDCreateTable(sql_statement);
            TraverseGDD(gdd);
            saveTableToEtcd(gdd);
            vector<string> sitename;
            vector<string> sql;
            sitename.clear();
            sql.clear();
            sitename.push_back("s1");
            string sql_tmp = GetBefore(sql_statement,"[") + "(" + GetExactAfter(sql_statement,"[");
            sql_tmp = GetBefore(sql_tmp,"]") + ")";
            sql.push_back(sql_tmp);
            cout << sql_tmp << "/" << endl;
            Data_Insert_Delete_Execute(sitename,sql); 
        }
        else if (sql_statement.find("CREATE FRAGMENTATION") != -1) {
            // cout << "CREATE FRAGMENTATION" << endl;
            Fragment fragment = InitGetFragmentCreateFragment(sql_statement);
            TraverseFragment(fragment);
            saveFragToEtcd(fragment);
            map<string,GDD> GDDMap = GetETCDGDD();
            string table_name = GetBetween(sql_statement,"FRAGMENTATION","(");
            GDD gdd = GDDMap[table_name];
            if (fragment.fragtype == "H") {
                string collumns = gdd.cols[0].name + " " + gdd.cols[0].type;
                if (gdd.cols[0].key == true) {
                    collumns = collumns + "key";
                }
                for (int i = 1; i < gdd.cols.size(); i++) {
                    collumns = collumns + "," + gdd.cols[i].name + " " + gdd.cols[i].type;
                    if (gdd.cols[i].key == true) {
                        collumns = collumns + " key";
                    }
                }
                collumns = "(" + collumns + ")";
                vector<string> sitename;
                vector<string> sql;
                sitename.clear();
                sql.clear();

                for (int i = 0; i < fragment.frags.size(); i++) {
                    sitename.push_back("s" + to_string(fragment.frags[i].id));
                    sql.push_back("CREATE TABLE " + table_name +"_" + to_string(fragment.frags[i].id) + collumns);
                }
                Traverse(sitename);
                Traverse(sql);
                cout << Data_Insert_Delete_Execute(sitename,sql);
            }
            else if (fragment.fragtype == "V") {
                cout << "Fragment.fragtype = V" << endl;
                vector<string> sitename;
                vector<string> sql;
                sitename.clear();
                sql.clear();
                
                for (int i = 0; i < fragment.frags.size(); i++) {
                    string collumn_select;
                    string collumns = fragment.frags[i].condition;
                    cout << "collumn :" << collumns << "/" << endl;
                    vector<string> collumns_list = GetList(collumns,"AND","?");
                    Traverse(collumns_list);
                    for (int j = 0; j < collumns_list.size(); j++) {
                        collumns_list[j] = GetExactAfter(collumns_list[j],".");
                    }
                    Traverse(collumns_list);
                    for (int j = 0; j < gdd.cols.size(); j++) {
                        if (JudgeHit(collumns_list,gdd.cols[j].name)) {
                            cout << "Hit :" << gdd.cols[j].name << endl;
                            if (collumn_select == "") {
                                collumn_select = collumn_select + gdd.cols[j].name + " " + gdd.cols[j].type;
                                if (gdd.cols[j].key==true) {
                                    collumn_select = collumn_select + " key";
                                }
                            }
                            else {
                                collumn_select = collumn_select + "," +  gdd.cols[j].name + " " + gdd.cols[j].type;
                                if (gdd.cols[j].key==true) {
                                    collumn_select = collumn_select + " key";
                                }
                            }
                        }
                    }
                    collumn_select = "(" + collumn_select + ")";
                    cout << collumn_select << "/" << endl;
                    sitename.push_back("s" + to_string(fragment.frags[i].id));
                    sql.push_back("CREATE TABLE " + table_name + "_" + to_string(fragment.frags[i].id) + collumn_select);
                }
                Traverse(sitename);
                Traverse(sql);
                cout << Data_Insert_Delete_Execute(sitename,sql);
            }
            else {
                cout << "INVALID FRAGMENT TYPE " << endl;
            }
            
        }
        else if (sql_statement.find("LOCAL CREATE EMPTY TABLE :") != -1) {
            // create_sql_yq = GetLocalCreate(sql_statement);
            // Data_Load_Execute(create_sql_yq,NULL,NULL,NULL,NULL,NULL);
            // cout << result << endl;
        }
        else if (sql_statement.find("SELECT") != -1) {
            time_t myt = time(NULL);
            cout << "myt :" << myt << "/" << endl;
            TREE Tree = SELECT(sql_statement,myt);
            cout << "TREE >>>" << endl;
            TraverseTree(Tree.Nodes);
            exec_tree exe = Data_Select_Execute(Tree);
            for (int i = 0; i < exe.Nodes.size(); i++) {
                cout << "node_id :" << exe.Nodes[i].node_id << endl ;
                cout << "time_spend :" << exe.Nodes[i].time_spend << endl;
                cout << "volume :" << exe.Nodes[i].volume << endl;
                cout << "res :" << exe.Nodes[i].res << endl;
                cout << "child :" << endl;
                TraverseInt(exe.Nodes[i].child);
                cout << "parent :" << exe.Nodes[i].parent << endl;
                cout << "site :" << exe.Nodes[i].site << endl;
            }
            DrawTree(Tree.Nodes);
            ofstream out("the result of select.txt"); 
            if (out.is_open()) {
                out << "The result of select is as follow :\n";
                // my_mysql_res_print("tree_"+to_string(myt)+"_node_"+to_string(exe.root));
                out.close();    
            }
            cout << "rows: " << my_mysql_res_get_rows("tree_"+to_string(myt)+"_node_"+to_string(exe.root)) << endl;
        }
        else if (sql_statement.find("INSERT") != -1) {
            vector<string> sitename;
            vector<string> sql;
            sitename = GetsitenameInsert(sql_statement);
            sql = GetsqlInsert(sql_statement);
            cout << "sitename>>>" << endl;
            Traverse(sitename);
            cout << "sql>>>" << endl;
            Traverse(sql);
            cout << Data_Insert_Delete_Execute(sitename,sql);
        }
        else if (sql_statement.find("DELETE") != -1) {
            vector<string> sitename;
            vector<string> sql;
            string table_name;
            if (sql_statement.find("WHERE") != -1) {
                table_name = GetBetween(sql_statement, "TABLE", " WHERE");
            }
            else {
                table_name = GetBetween(sql_statement, "TABLE", ";");
            }
            map<string,Fragment> FragmentMap = GetETCDFragment();
            Fragment fragment = FragmentMap[table_name];
            map<string,string> TableKeyMap = GetETCDTableKey();
            string type = fragment.fragtype;
            string key = TableKeyMap[table_name];
            cout << "table_name :" << table_name << "/" << endl;
            if (type == "H") {
                cout << "type == H" << endl;
                sitename = GetsitenameDelete(sql_statement);
                sql = GetsqlDelete(sql_statement);
                cout << "sitename>>>" << endl;
                Traverse(sitename);
                cout << "sql>>>" << endl;
                Traverse(sql);
                
                cout << Data_Insert_Delete_Execute(sitename,sql);   
            }
            else if (type == "V") {
                cout << "type == V" << endl;
                if (sql_statement.find("WHERE") == -1) {
                    vector<string> sql_statement_tmp;
                    vector<string> sitename_tmp;
                    sql_statement_tmp.clear();
                    sitename_tmp.clear();
                    for (int i = 0; i < fragment.frags.size(); i++) {
                        string sitename_tmp_item = "s" + to_string(fragment.frags[i].site);
                        sitename_tmp.push_back(sitename_tmp_item);
                        string sql_statement_tmp_item = "DELETE FROM " + table_name + "_" + to_string(fragment.frags[i].site);
                        sql_statement_tmp.push_back(sql_statement_tmp_item);
                        cout << sql_statement_tmp_item << "/" << endl;
                        cout << sitename_tmp_item << "/" << endl;
                    }
                    cout << Data_Insert_Delete_Execute(sitename_tmp, sql_statement_tmp);
                }
                else if (sql_statement.find("WHERE") != -1){
                    string select_sql_statement = GetSelectSql(sql_statement);
                    cout << "select_sql_statement :" << select_sql_statement << "/" << endl;
                    time_t myt = time(NULL);
                    TREE Tree = SELECT(select_sql_statement,myt);
                    exec_tree exe = Data_Select_Execute(Tree);
                    vector<int> id_list = my_mysql_res_get_column1("tree_"+to_string(myt)+"_node_"+to_string(exe.root));
                    // cout << "id_list.size() :" << id_list.size() << endl;
                    // for (int k = 0; k < id_list.size(); k++) {
                    //     cout << k << ":" << id_list[k] << endl; 
                    // }
                    // "tree_"+to_string(myt)+"_node_"+to_string(exe.root)
                    // cout << "contain WHERE" << endl;
                    vector<string> sql_statement_tmp;
                    vector<string> sitename_tmp;
                    sql_statement_tmp.clear();
                    sitename_tmp.clear();
                    for (int i = 0; i < fragment.frags.size(); i++) {
                        for (int j = 0; j < id_list.size(); j++) {
                            string sql_statement_tmp_item = "DELETE FROM " + table_name + "_" + to_string(fragment.frags[i].site) + " where " + key + "=" + to_string(id_list[j]);
                            cout << sql_statement_tmp_item << "/" << endl;
                            sql_statement_tmp.push_back(sql_statement_tmp_item);
                            string sitename_tmp_item = "s" + to_string(fragment.frags[i].site);
                            cout << sitename_tmp_item << "/" << endl;
                            sitename_tmp.push_back(sitename_tmp_item);
                        }
                    }
                    cout << Data_Insert_Delete_Execute(sitename_tmp, sql_statement_tmp);            
                }
            }
            else {
                cout << "INVALID FRAGMENT TYPE" << endl;
            }            
        }
        else if (sql_statement == "QUIT") {
            break;
        }
        else if (sql_statement.find("DROP") != -1) {
            vector<string> sql;
            vector<string> sitename;
            string table_name = GetBetween(sql_statement,"TABLE",";");
            string sitename_item = "s" + GetExactAfter(table_name,"_");
            sql.push_back(sql_statement);
            sitename.push_back(sitename_item);
            cout << Data_Insert_Delete_Execute(sitename,sql);
        }
        else {
            cout << "INVALID SQL STATEMENT " << endl;
            continue;
        }
    }

    return 0;
}