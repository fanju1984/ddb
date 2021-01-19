#include "Parse_SELECT.h"
using namespace std;

// int main() {  
//     string sql_statement;
//     TREE Tree;
//     sql_statement = "SELECT * FROM customer;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT publisher.name FROM publisher;"; // cannot ok without where
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT book.title FROM book WHERE book.copies>5000;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT orders.customer_id,orders.quantity FROM orders WHERE orders.quantity<8;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT book.title,book.copies,publisher.name,publisher.nation FROM book,publisher WHERE book.publisher_id=publisher.id and publisher.nation='USA' and book.copies>1000;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT customer.name,orders.quantity FROM customer,orders WHERE customer.id=orders.customer_id;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT customer.name,orders.rank,orders.quantity FROM customer,orders WHERE customer.id=orders.customer_id and customer.rank=1;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT customer.name,orders.quantity,book.title FROM customer,orders,book WHERE customer.id=orders.cusomer_id and book.id=orders.book_id and customer.rank=1 and book.copies>5000;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT customer.name,book.title,publisher.name,orders.quantity FROM customer,book,publisher,orders WHERE customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and book.id>220000 and publisher.nation='USA' and orders.quantity>1;";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     sql_statement = "SELECT customer.name,book.title,publisher.name,orders.quantity FROM customer,book,publisher,orders WHERE customer.id=orders.customer_id and book.id=orders.book_id and book.publisher_id=publisher.id and customer.id>308000 and book.copies>100 and orders.quantity>1 and publisher.nation='PRC';";
//     Tree = SELECT(sql_statement,0);
//     cout << sql_statement << "/" << endl;
//     TraverseTree(Tree.Nodes);
//     // cout << Tree.root << "/" << endl;
//     return 0;
// }
TCC GetTCC(string table_name, vector<string> column_list, vector<string> select_list) {
    // Get TCC for a table
   TCC tcc;
    for (int i = 0; i < column_list.size(); i++) {
        string table_tmp = GetBefore(column_list[i], ".");
        if  (table_tmp == "") {
            tcc.column_list.push_back(table_name + "." + column_list[i]);
        }
        if (table_tmp == table_name && JudgeNotin(tcc.column_list,column_list[i])) { 
            tcc.column_list.push_back(column_list[i]);
        }
    }
    for (int i = 0; i < select_list.size(); i++) {
        string table_tmp = GetTableFromSelectCondition(select_list[i]);
        if (table_tmp == table_name) {
            if (select_list[i].find(" ") != -1) {
                select_list[i] = GetBefore(select_list[i]," ");
            }
            tcc.condition_list.push_back(select_list[i]);
        }
    }
    // tcc.fratype = getTableFragType(table_name); // NEED TUTU
    tcc.table_name = table_name;
    return tcc;
}
vector<TCC> GetTCCList(vector<string> table_list, vector<string> column_list, vector<string> select_list) {
    vector<TCC> tcc_list;
    for (int i = 0; i < table_list.size(); i++) {
        TCC tcc = GetTCC(table_list[i], column_list, select_list);
        tcc_list.push_back(tcc);
    }
    return tcc_list; 
}
vector<string> GetSelectConditionList(vector<string> condition_list, vector<string> table_list) {
    vector<string> selectcondition_list;
    vector<string> tmp;
    vector<string> joincondition_list;
    // vector<string> condition_list = GetAllColumnList(sql_statement);
    for (int i = 0; i < condition_list.size(); i++) {
        tmp = GetColumnFromCondition(condition_list[i], table_list);
        if (tmp.size() == 2) {
            joincondition_list.push_back(condition_list[i]);
        }
        else if (tmp.size() == 1) {
            selectcondition_list.push_back(condition_list[i]);
        }
        else if (tmp.size() == 0) { // 当select语句中只涉及一个表，tmp.size() = 0，需要补齐这个condition的column名
            if (table_list.size() == 1) {
                condition_list[i] =  table_list[0] + "." + condition_list[i];             
            }
            selectcondition_list.push_back(condition_list[i]);
        }
        else {
            cout << "tmp.size() IN GetSelectConditionList :" << tmp.size() << endl;
            cout << "ERROR: " << condition_list[i] << endl;
        }
    }
    return selectcondition_list;
}
vector<string> GetJoinConditionList(vector<string> condition_list, vector<string> table_list) {
    vector<string> selectcondition_list;
    vector<string> tmp;
    vector<string> joincondition_list;
    // vector<string> condition_list = GetAllColumnList(sql_statement);
    for (int i = 0; i < condition_list.size(); i++) {
        tmp = GetColumnFromCondition(condition_list[i], table_list);
        if (tmp.size() == 2) {
            joincondition_list.push_back(condition_list[i]);
        }
        else if (tmp.size() == 1) {
            selectcondition_list.push_back(condition_list[i]);
        }
        else {
            cout << "tmp.size() IN GetJoinConditionList :" << tmp.size() << endl;
            cout << "ERROR: " << condition_list[i] << endl;
        }
    }
    return joincondition_list;
}
bool JudgeNotin(vector<string> BigList, string item) {
    for (int i = 0; i < BigList.size(); i++) {
        if (item == BigList[i]){
            return false;
        }
    }
    return true;
}
bool JudgeNotinInt(vector<int> BigList, int item) {
    for (int i = 0; i < BigList.size(); i++) {
        if(item == BigList[i]) {
            return false;
        }
    }
    return true;
}
void TraverseTCCList(vector<TCC> TCCList) {
    for (int i = 0; i < TCCList.size(); i++) {
        cout << "table_name :" << TCCList[i].table_name << "/" << endl;
        cout << "column_list >>> " << endl;
        Traverse(TCCList[i].column_list);
        cout << "condition_list >>> " << endl;
        Traverse(TCCList[i].condition_list);
        cout << "fratype :" << TCCList[i].fratype << "/" << endl;
    }
}
string GetTableFromSelectCondition(string select_condition) {
    string column;
    string table;
    if (select_condition.find("=") != -1) {
        column = GetBefore(select_condition,"=");
        table = GetBefore(column,".");
    }
    else if (select_condition.find(">") != -1) {
        column = GetBefore(select_condition,">");
        table = GetBefore(column,".");
    }
    else if (select_condition.find("<") != -1) {
        column = GetBefore(select_condition,"<");
        table = GetBefore(column,".");
    }
    else if (select_condition.find(">=") != -1) {
        column = GetBefore(select_condition,">=");
        table = GetBefore(column,".");
    }
    else if (select_condition.find("<=") != -1) {
        column = GetBefore(select_condition,"<=");
        table = GetBefore(column,".");
    }
    return table;
}
vector<string> GetTableFromJoinCondition(string join_condition) {
    vector<string> table_list;
    if (join_condition.find("=") != -1) {
        string column_1 = GetBefore(join_condition,"=");
        string column_2 = GetExactAfter(join_condition,"=");
        table_list.push_back(GetBefore(column_1,"."));
        table_list.push_back(GetBefore(column_2,"."));
    }
    return table_list;
}
vector<int> GetNodeListFromTableList(vector<string> table_in_join_condition, vector<TABLE> TableList) {
    vector<int> node_list;
    for(int i = 0; i < table_in_join_condition.size(); i++) {
        for (int j = 0; j < TableList.size(); j++) {
            if (table_in_join_condition[i] == TableList[j].table_name) {
                node_list.push_back(TableList[j].node_id);
            }
        }
    }
    return node_list;
}
vector<TCC> GetTCCListTest(string sql_statement) {
    // metadata //
    map<string,Fragment> FragmentMap = GetETCDFragment();
    vector<string> table_list = GetTableList(sql_statement);
    vector<string> column_list = GetAllColumnList(sql_statement);
    string condition = GetCondition(sql_statement);
    // cout << "condition :" << condition << "/" << endl;
    vector<string> condition_list = GetConditionList(sql_statement);
    vector<string> select_list = GetSelectConditionList(condition_list,table_list);
    vector<TCC> tccList = GetTCCList(table_list, column_list, select_list);
    cout << "condition_list in GetTCCListTest>>>" << endl;
    Traverse(condition_list);
    for (int i = 0; i < tccList.size(); i++) {
        // tccList[i].fratype = getFragFromEtcd(table_list[i]).fragtype;
        tccList[i].fratype = FragmentMap[table_list[i]].fragtype;
    }
    return tccList;
}
TREE SELECT(string sql_statement, int treeid) {
    // vector<string> all_collumn_list = GetAllColumnList(sql_statement);
    // cout << "all_collumn_list>>>" << endl;
    // Traverse(all_collumn_list);
    map<string,Fragment> FragmentMap = GetETCDFragment();
    vector<TCC> TCCList = GetTCCListTest(sql_statement);
    map<string,string> TableKeyMap = GetETCDTableKey();
    TREE Tree;
    NODE node;
    vector<NODE> tree;
    Tree.tree_id = treeid;
    int iid = 1;
    map<string,int> TableMap;
    // cout << "TCCList>>>" << endl;
    // TraverseTCCList(TCCList);
    // GetData
    for (int i = 0; i < TCCList.size(); i++) {
        string table_name = TCCList[i].table_name;
        cout << "table_name in SELECT :" << table_name << "/" << endl;
        if (TCCList[i].fratype == "H") {
            vector<string> condition_list = TCCList[i].condition_list;
            string condition;
            // cout << "condition_list>>>" << endl;
            // Traverse(condition_list);
            if (condition_list.size() == 0) {
                condition = "";
            }
            else {
                vector<string> leaf_condition_list = GetLeafConditionList(condition_list);
                condition = " WHERE " + Link(leaf_condition_list, " AND ");                
            }
            // Fragment fragment = getFragFromEtcd(table_name);
            Fragment fragment = FragmentMap[table_name];
            vector<FragDef> frags = fragment.frags;
            for (int j = 0; j < frags.size(); j++) {
                node.id = iid;
                node.site = frags[j].site;
                node.child.clear();
                vector<string> select_content_list = GetAllDifferentCollumnListOfTable(sql_statement,table_name);
                string select_content = Link(select_content_list,",");
                node.sql_statement = "SELECT " + select_content + " FROM " + table_name + "_" + to_string(frags[j].site) + condition; //叶子节点 不带表名
                iid += 1;
                tree.push_back(node);
            }

            node.id = iid;
            node.site = 1;
            node.child.clear();
            for (int k = 1; k <= frags.size(); k++) {
                node.child.push_back(iid-k);
            }
            node.sql_statement = "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(node.child[0]);
            for (int k = 1; k < frags.size(); k++) {
                node.sql_statement += " UNION ALL SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(node.child[k]);
            }
            TableMap[table_name] = iid;          
            iid += 1;
            tree.push_back(node);
        }
        else if (TCCList[i].fratype == "V") {
            Fragment fragment = FragmentMap[table_name];
            map<string,string> TableKeyMap = GetETCDTableKey();
            // Fragment fragment = getFragFromEtcd(table_name);
            vector<FragDef> frags = fragment.frags;
            // string key = getTableKey(table_name);
            string key = TableKeyMap[table_name];
            // cout << frags.size() << endl;
            // cout << "fragtype :"<< fragment.fragtype << "/" << endl;
            // cout << "fragnum :" << fragment.fragnum << "/" << endl;
            for (int j = 0; j < frags.size(); j++) {
                node.id = iid;
                node.site = frags[j].site;
                node.child.clear();
                node.sql_statement = "SELECT * FROM " + table_name + "_" + to_string(node.site);
                iid += 1;
                tree.push_back(node);
            }
            string join_condition;
            for (int k = 1; k < frags.size(); k++) { 
                join_condition = Get_join_condition(treeid,iid-1,iid-2*k,key);
                node.id = iid;
                node.site = 1;
                node.sql_statement = join_condition;
                node.child.clear();
                node.child.push_back(iid-1);
                node.child.push_back(iid-2*k);
                iid += 1;
                tree.push_back(node);
            }
            if (TCCList[i].condition_list.size() != 0) {
                node.id = iid;
                node.site = 1;
                vector<string> purecondition_list;
                for (int j = 0; j < TCCList[i].condition_list.size(); j++) {
                    purecondition_list.push_back(GetExactAfter(TCCList[i].condition_list[j],"."));
                }
                node.sql_statement = "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(iid-1) + " WHERE " +  Link(purecondition_list," and ");
                node.child.clear();
                node.child.push_back(iid-1);
                iid += 1;
                tree.push_back(node);
            }

            TableMap[table_name] = iid-1; 
        }
        else {
            cout << "INVALID FRAGTYPE :" << TCCList[i].fratype << endl; 
        }
    }
    // cout << "GetData IN SELECT " << endl;
    // Join
    vector<string> condition_list = GetConditionList(sql_statement);
    // cout << "condition_list>>>" << endl;
    // Traverse(condition_list);
    vector<string> table_list = GetTableList(sql_statement);
    vector<string> join_condition_list = GetJoinConditionList(condition_list,table_list);
    // Traverse(table_list);
    // cout << "join_condition_list>>>" << endl;
    // Traverse(join_condition_list);

    // TraverseTableMap(TableMap);
    vector<string> table_list_joined;
    vector<string> all_collumn_list;
    vector<string> all_collumn_list_2;

    for (int i = 0; i < join_condition_list.size(); i++) {
        // cout << "join_condition :" << join_condition_list[i] << "/" << endl;
        string column_1 = GetBefore(join_condition_list[i],"=");
        string column_2 = GetExactAfter(join_condition_list[i],"=");
        string table_1 = GetBefore(column_1,".");
        string table_2 = GetBefore(column_2,".");
        string pure_column_1 = GetExactAfter(column_1,".");
        string pure_column_2 = GetExactAfter(column_2,".");
        vector<string> table_list_joined_tmp;
        table_list_joined_tmp.clear();
        table_list_joined_tmp.push_back(table_1);
        table_list_joined_tmp.push_back(table_2);
        for (int j = 0; j < table_list_joined_tmp.size(); j++){
            if (!JudgeHit(table_list_joined, table_list_joined_tmp[j])) {
                table_list_joined.push_back(table_list_joined_tmp[j]);
            }
        }
        // if (pure_column_2.find(" ") == 0) {
        //     pure_column_2 = GetExactAfter(pure_column_2," ");
        // }
        // else if (pure_column_2.find(" ") != 0) {
        //     pure_column_2 = GetBefore(pure_column_2," ");
        // }
        // cout << "column_1 :" << column_1 << "/" << endl;
        // cout << "column_2 :" << column_2 << "/" << endl;
        // cout << "table_1 :" << table_1 << "/" << endl;
        // cout << "table_2 :" << table_2 << "/" << endl; 
        // cout << "pure_column_1 :" << pure_column_1 << "/" << endl;
        // cout << "pure_column_2 :" << pure_column_2 << "/" << endl;
        node.id = iid;
        node.child.clear();
        node.child.push_back(TableMap[table_1]);
        node.child.push_back(TableMap[table_2]);
        string treenode1 = GetTreeNode(treeid,TableMap[table_1]);
        string treenode2 = GetTreeNode(treeid,TableMap[table_2]);
        all_collumn_list_2 = GetCollumnsSelect(table_list_joined,TCCList);
        // all_collumn_list_2 = GetCleanList(all_collumn_list_2);
        string all_collumn = Link(all_collumn_list_2,",");
        // string all_collumn = Getall_collumn(table_1,treenode1,table_2,treenode2);
        // cout << "all_collumn :" << all_collumn << "/" << endl; 
        node.sql_statement = "SELECT "+ all_collumn + " FROM tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_1]) + ",tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_2]) + " WHERE " + "tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_1]) + "." + pure_column_1 + "=" + "tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_2]) + "." + pure_column_2; 
        TableMap[table_1] = iid;
        TableMap[table_2] = iid;
        tree.push_back(node);
        iid += 1;
    }
    cout << "join done" << endl;
    // FinalPI
    if (sql_statement.find("*") == -1) {
        node.id = iid;
        node.child.clear();
        node.child.push_back(iid-1);
        // node.parent = 0;
        vector<string> select_column = GetCollumnFromSql(sql_statement);
        // cout << "select_column>>>" << endl;
        // Traverse(select_column);
        // select_column = GetInerColumnList(select_column,treeid, iid-1);
        // string select = Link(select_column, ",");
        // cout << "select :" << select << endl;
        // cout << "select_column>>>" << endl;
        Traverse(select_column);
        string select = Link(select_column, ",");
        node.sql_statement = "SELECT " + select + " FROM tree_" + to_string(treeid) + "_node_" + to_string(iid-1); // 中间节点的表名是node名
        iid += 1;
        tree.push_back(node);
    }
    // cout << "final pi" << endl;
    // GetParent for result of GetData
    for (int i = 0; i < tree.size(); i++) {
        int node_id = tree[i].id;
        for(int j = i+1; j < tree.size(); j++) {
            if (JudgeNotinInt(tree[j].child, node_id) == 0) {
                tree[i].parent = tree[j].id;
                break;
            }
        }
    }
    tree[tree.size()-1].parent = 0;
    Tree.root = iid-1;
    Tree.Nodes = tree;
    return Tree;
}
TREE SELECTLocal(string sql_statement,int treeid,map<string,Fragment> FragmentMap,map<string,string> TableKeyMap) {
    vector<TCC> TCCList = GetTCCListTest(sql_statement);
    // TraverseTCCList(TCCList);
    // cout << "TraverseTCCList DONE " << endl; 
    TREE Tree;
    NODE node;
    vector<NODE> tree;
    Tree.tree_id = treeid;
    int iid = 1;
    map<string,int> TableMap;
    // GetData
    for (int i = 0; i < TCCList.size(); i++) {
        string table_name = TCCList[i].table_name;
        if (TCCList[i].fratype == "H") {
            vector<string> condition_list = TCCList[i].condition_list;
            string condition = Link(condition_list, " AND ");
            cout << "condition :" << condition << "/" << endl;
            Fragment frag = getFragFromEtcd(table_name);
            // cout << "table_name :" << table_name << "/" << endl;
            Fragment fragment = FragmentMap[table_name];
            vector<FragDef> frags = fragment.frags;
            for (int j = 0; j < frags.size(); j++) {
                // cout << "j =" << j << endl;
                node.id = iid;
                node.site = frags[j].site;
                node.child.clear();
                node.sql_statement = "SELECT * FROM " + table_name + "_" + to_string(frags[j].site) + " WHERE " +  condition;
                iid += 1;
                tree.push_back(node);
            }

            node.id = iid;
            node.site = 1;
            node.child.clear();
            for (int k = 1; k <= frags.size(); k++) {
                node.child.push_back(iid-k);
            }
            node.sql_statement = "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(node.child[0]);
            for (int k = 1; k < frags.size(); k++) {
                node.sql_statement += " UNION ALL SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(node.child[k]);
            }
            TableMap[table_name] = iid;          
            iid += 1;
            tree.push_back(node);
        }
        else if (TCCList[i].fratype == "V") {
            // cout << TCCList[i].fratype << "/" << endl;
            Fragment fragment = FragmentMap[table_name];
            // Fragment frag = getFragFromEtcd(table_name);
            vector<FragDef> frags = fragment.frags;
            // string key = getTableKey(table_name);
            string key = TableKeyMap[table_name];
            // cout << "key :" << key << "/" << endl;
            // cout << frags.size() << endl;
            // cout << "fragtype :"<< fragment.fragtype << "/" << endl;
            // cout << "fragnum :" << fragment.fragnum << "/" << endl;
            for (int j = 0; j < frags.size(); j++) {
                node.id = iid;
                node.site = frags[j].site;
                node.child.clear();
                node.sql_statement = "SELECT * FROM " + table_name + "_" + to_string(node.site);
                iid += 1;
                tree.push_back(node);
            }
            string join_condition;
            for (int k = 1; k < frags.size(); k++) { 
                join_condition = Get_join_condition(treeid,iid-1,iid-2*k,key);
                node.id = iid;
                node.site = 1;
                node.sql_statement = join_condition;
                node.child.clear();
                node.child.push_back(iid-1);
                node.child.push_back(iid-2*k);
                iid += 1;
                tree.push_back(node);
            }
            TableMap[table_name] = iid-1; 
        }
        else {
            cout << "INVALID FRAGTYPE :" << TCCList[i].fratype << endl; 
        }
    }
    // cout << "GetData IN SELECT " << endl;
    // Join
    vector<string> condition_list = GetConditionList(sql_statement);
    vector<string> table_list = GetTableList(sql_statement);
    vector<string> join_condition_list = GetJoinConditionList(condition_list,table_list);
    // Traverse(join_condition_list);
    // Traverse(table_list);
    // TraverseTableMap(TableMap);

    for (int i = 0; i < join_condition_list.size(); i++) {
        string column_1 = GetBefore(join_condition_list[i],"=");
        string column_2 = GetExactAfter(join_condition_list[i],"=");
        string table_1 = GetBefore(column_1,".");
        string table_2 = GetBefore(column_2,".");
        string pure_column_1 = GetExactAfter(column_1,".");
        string pure_column_2 = GetExactAfter(column_2,".");
        pure_column_2 = GetBefore(pure_column_2," ");
        // cout << column_1 << " " << column_2 << endl;
        // cout << table_1 << " " << table_2 << endl;
        // cout << "pure_column_1 :" << pure_column_1 << "/" << endl;
        // cout << "pure_column_2 :" << pure_column_2 << "/" << endl;
        node.id = iid;
        node.child.clear();
        node.child.push_back(TableMap[table_1]);
        node.child.push_back(TableMap[table_2]);
        node.sql_statement = "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_1]) + ",tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_2]) + " WHERE " + to_string(treeid) + "_node_" + to_string(TableMap[table_1]) + "." + pure_column_1 + "=" + "tree_" + to_string(treeid) + "_node_" + to_string(TableMap[table_2]) + "." + pure_column_2;
        TableMap[table_1] = iid;
        TableMap[table_2] = iid;
        tree.push_back(node);
        iid += 1;
    }
    // cout << "JOIN IN SELECT" << endl;
    // FinalPI
    node.id = iid;
    node.child.clear();
    node.child.push_back(iid-1);
    node.parent = 0;
    vector<string> select_column = GetSelectColumnList(sql_statement);
    string select = Link(select_column, ",");
    node.sql_statement = "SELECT " + select + " FROM tree_" + to_string(treeid) + "_node_" + to_string(iid-1);
    iid += 1;
    tree.push_back(node);
    // // GetParent for result of GetData
    for (int i = 0; i < tree.size(); i++) {
        int node_id = tree[i].id;
        for(int j = i+1; j < tree.size(); j++) {
            if (JudgeNotinInt(tree[j].child, node_id) == 0) {
                tree[i].parent = tree[j].id;
                break;
            }
        }
    }
    tree[tree.size()-1].parent = 0;
    Tree.root = iid-1;
    Tree.Nodes = tree;
    return Tree;
}
void TraverseTree(vector<NODE> tree) {
    for(int i = 0; i < tree.size(); i++) {
        cout << "=="<< tree[i].id << "==" << endl;
        cout << "site =>" << tree[i].site << endl;
        cout << "sql_statement =>" << tree[i].sql_statement<< "/" << endl;
        cout << "child =>" << endl;
        TraverseInt(tree[i].child);
        cout << "parent =>" << endl;
        cout << tree[i].parent << endl;
    }
}
void TraverseInt(vector<int> intlist) {
    for (int i = 0; i < intlist.size(); i++){
        cout << intlist[i] << endl;
    }
}
void TraverseTableMap(map<string,int> TableMap) {
    map<string, int>::iterator iter;
    iter = TableMap.begin();
    while(iter != TableMap.end()) {
        cout << iter->first << " : " << iter->second << endl;
        iter++;
    }
}
vector<string> GetKeyofTable(string table_name) {
    vector<string> keys;
    if (table_name == "Customer") {
        keys.push_back("id");
    }
    if (table_name == "Book") {
        keys.push_back("id");    
    }
    if (table_name == "Publisher") {
        keys.push_back("id");    
    }
    if (table_name == "Orders") {        
        keys.push_back("Customer_id");
        keys.push_back("Book_id");
    }
    return keys;
}
vector<string> GetFullKeyofTable(string table_name) {
    vector<string> keys = GetKeyofTable(table_name);
    vector<string> fullkeys;
    for (int i = 0; i < keys.size(); i++) {
        fullkeys.push_back(table_name + "." +keys[i]);
    }
    return fullkeys;
}
string Get_join_condition(int treeid, int a, int b, string key) {
    // return "SELECT * FROM " + film JOIN film_actor USING (film_id)
    return "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(a) + " JOIN tree_" + to_string(treeid) + "_node_" + to_string(b) + " USING ("+ key + ")"; 
    // return "SELECT * FROM tree_" + to_string(treeid) + "_node_" + to_string(a) + ",tree_" + to_string(treeid) + "_node_" + to_string(b) + " WHERE " + "tree_" + to_string(treeid) + "_node_" + to_string(a) + "." + key + "=" + "tree_" + to_string(treeid) + "_node_" + to_string(b) + "." + key; 
}
vector<string> GetLeafConditionList(vector<string> condition_list) {
    for (int i = 0; i < condition_list.size(); i++) {
        condition_list[i] = GetExactAfter(condition_list[i],".");
    }
    return condition_list;
}
vector<string> GetInerColumnList(vector<string> column_list, int treeid, int nodeid) {
    for (int i = 0; i < column_list.size(); i++) {
        column_list[i] = "tree_" + to_string(treeid) + "_node_" + to_string(nodeid) + "." + GetExactAfter(column_list[i],".");
    }
    return column_list;
}

vector<string> GetCollumnFromSql(string sql_statement) {
    vector<string> collumn_list;
    collumn_list = GetTableCollumnFromSql(sql_statement);
    for (int i = 0; i < collumn_list.size(); i++) {
        collumn_list[i] = GetExactAfter(collumn_list[i],".");
    }
    collumn_list = GetCleanList(collumn_list);
    return collumn_list;    
}
vector<string> GetCollumnsSelect(vector<string> table_list, vector<TCC> TCCList) {
    vector<string> collumn_list;
    for (int i = 0; i < table_list.size(); i++) {
        for (int j = 0; j < TCCList.size(); j++) {
            if (TCCList[j].table_name == table_list[i]) {
                for (int k = 0; k < TCCList[j].column_list.size(); k++ ){
                    string item = GetExactAfter(TCCList[j].column_list[k],".");
                    collumn_list.push_back(item);
                }
                
            }
        }
    }
    collumn_list = GetCleanList(collumn_list);
    return collumn_list;
}
vector<string> GetTableCollumnFromSql(string sql_statement) {
    vector<string> tablecollumn_list;
    string tablecollumn = GetBetween(sql_statement, "SELECT"," FROM");
    cout << "tablecollumn " << tablecollumn << "/" << endl;
    tablecollumn_list = GetList(tablecollumn,",",";");
    return tablecollumn_list;
}
void DrawTree(vector<NODE> Nodes) {
    ofstream out("drawtree.gv");
    if (out.is_open()) {
        out << "digraph G {" << endl;
        for (int i = 0; i < Nodes.size(); i++) { // id site sql_statement child
            string item = to_string(Nodes[i].id) + "[label=" + to_string(Nodes[i].id);
            item = item + ",label=\"" + Nodes[i].sql_statement + "\"";
            int site = Nodes[i].site;
            switch (site) {
                case 1:
                    item = item + ",color=red];";
                    break;
                case 2:
                    item = item + ",color=yellow];";
                    break;
                case 3:
                    item = item + ",color=green];";
                    break;
                case 4:
                    item = item + ",color=blue];";
                    break;
            }
            out << item << endl;
            for (int j = 0; j < Nodes[i].child.size(); j++) {
                string tmp = to_string(Nodes[i].child[j])+ "->"+to_string(Nodes[i].id)+";";
                out << tmp << endl;
            }
        }
        out << "}" << endl;
        out.close();
    }
    else {
        cout << "ERROR : drawtree.gv is not open" << endl;
    }
}