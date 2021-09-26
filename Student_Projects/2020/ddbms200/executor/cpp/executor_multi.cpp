#include "./executor_multi.h"
// #include<stdio.h>
// #include<stdlib.h>
// #include<sys/stat.h>
// #include <thread>
// #include <future>
// #include <utility>

// /* 这是为了记录每次最多并行多少线程，受到机器最大并行数量的限制，
// 所以实质上应当取max(机器允许的最大并行数量，数据库站点数量)，
// 但事实上后面如果有递归执行，可能会直接超出机器能力，
// 而且直接写在这里也有点笨，size算出来然后在内部申请可以更省空间 */
// #define MAXTHREAD 4
// #include <iostream>
// #include <string>
// #include <vector>
// #include <ctime>
// #include "./mysql_connector.h"
// #include "../../transfer/transfer/transfer.h"

// /* 在每个site实例上部署的时候这个值不同 */
// #define LOCALSITE "s1"

// /* 本函数用于执行整个load流程，输入本地创建和导入表的SQL语句，本地表的表名，站点列表，
// 分片select语句列表和分片表名列表，返回
// "x1 rows imported on site 1.
//  x2 rows imported on site2.
//  y seconds used."
//  或者"FAIL" */
// string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names);
// /* for循环内原先的内容被封装为另外一个函数，
// 输入sitenames, sqls, table_names, 输出String - "xx rows imported on site x.\n" 或者 "FAIL on site x.\n" 
// 最后三个参数是为了传递主函数给每个线程的对应变量预留的空间，而不是传递值 */
// void Data_Load_Thread(string site, string frag_sql, string frag_name, std::promise<string> &resultObj);
// /* 把int型的节点名称（1）转化为字符串型（"s1"） */
// string site_to_string(int site_num);
// /* 通过系统stat结构体获取文件大小，单位bytes，size_t为长整型，若要打印占位符为%ld */
// size_t get_filebytes(const char *filename);
// /* 输入一个计划树结构，返回这棵树的根节点 */
// NODE get_root(TREE tree);
// /* 输入一个执行树结构，返回这棵树的根节点 */
// exec_node get_root(exec_tree tree);
// /* 输入一个树结构和节点id，返回对应节点 */
// NODE get_node(TREE tree, int node_id);
// /* 输入一个树结构和节点id，返回以对应节点为根节点的子树 */
// TREE get_sub_tree(TREE tree, int node_id);
// /* RPC_Data_Select_Execute的可并行化版本，合并前先注释掉方便调试 */
// void RPC_Data_Select_Execute_Thread(TREE tree, string site, std::promise<exec_tree> &resultObj);
// /* Data_Select_Execute的可并行化版本 */
// void Data_Select_Execute_Thread(TREE tree, std::promise<exec_tree> &resultObj);
// /* 本函数用于递归执行整个select流程，输入一棵查询计划树，返回对应的执行结果树 
// 至于查询到的数据结果，从tree与node的id便可以推测得到结果表的名字，
// mysql_connector.h里面也提供了根据结果表名字打印结果的函数void my_mysql_res_print(string my_res);
// 注意此版本非并行化版本，仅供RPC_Data_Select_Execute或者parser调用（递归的开始），内部调用的均为可并行化版本 */
// exec_tree Data_Select_Execute(TREE tree);

// int main(int argc,char *argv[])
// {
//     /* 以下是测试LOAD所用代码 */
//     // /* id int key, name char(100), nation char(3) */
//     // string create_sql = "create table publisher(id int(6), name char(100), nation char(3))";
//     // string load_sql = "load data local infile '/home/roy/ddbms/rawdata/publisher.tsv' into table publisher";
//     // string main_name = "publisher";
//     // vector<string> sitenames;
//     // string site = "s3";
//     // sitenames.push_back(site);
//     // site = "s4";
//     // sitenames.push_back(site);
//     // site = "s2";
//     // sitenames.push_back(site);
//     // vector<string> sqls;
//     // string sql = "select * from publisher where id < 104000 and nation='PRC'";
//     // sqls.push_back(sql);
//     // sql = "select * from publisher where id > 104000 and nation='PRC'";
//     // sqls.push_back(sql);
//     // sql = "select * from publisher where id > 104000 and nation='PRC'";
//     // sqls.push_back(sql);
//     // vector<string> table_names;
//     // string table_name = "publisher_3";
//     // table_names.push_back(table_name);
//     // table_name = "publisher_4";
//     // table_names.push_back(table_name);
//     // table_name = "publisher_2";
//     // table_names.push_back(table_name);

//     // string load_output = Data_Load_Execute(create_sql, load_sql, main_name, sitenames, sqls, table_names);
//     // printf("%s", load_output.data());

//     /* 以下是测试SELECT所用代码 */
//     /* 此处应有tree定义 */   
//     // NODE tmp_input_node = new NODE();  
//     // NODE tmp_input_node1;
//     // vector<NODE> input_nodes;
//     // TREE Tree;
//     // Tree.root = 1;
//     // Tree.tree_id = 0;
//     // tmp_input_node1.id = 1;
//     // tmp_input_node1.site = 3;
//     // tmp_input_node1.sql_statement = "select tree_0_node_2.id from tree_0_node_2,tree_0_node_3 where tree_0_node_2.publisher_id = tree_0_node_3.id and tree_0_node_2.id > 249946";
//     // // tmp_input_node1.sql_statement = "select * from tree_0_node_2 where id < 100005";
//     // tmp_input_node1.child.push_back(2);
//     // tmp_input_node1.child.push_back(3);
//     // input_nodes.push_back(tmp_input_node1);

//     // NODE tmp_input_node2;
//     // tmp_input_node2.id = 2;
//     // tmp_input_node2.site = 3;
//     // tmp_input_node2.sql_statement = "select * from book";
//     // tmp_input_node2.parent = 1;
//     // tmp_input_node2.child.clear();
//     // input_nodes.push_back(tmp_input_node2);

//     // NODE tmp_input_node3;
//     // tmp_input_node3.id = 3;
//     // /* 如果本地调试用这两行 */
//     // // tmp_input_node3.site = 4;
//     // // tmp_input_node3.sql_statement = "select * from publisher_4";
//     // tmp_input_node3.site = 2;
//     // tmp_input_node3.sql_statement = "select * from publisher_2";

//     // tmp_input_node3.parent = 1;   
//     // tmp_input_node3.child.clear();
//     // input_nodes.push_back(tmp_input_node3);

//     // Tree.Nodes = input_nodes;
//     // // delete tmp_input_node;

//     // exec_tree res_tree = Data_Select_Execute(Tree);
//     // printf("tree id: %d\n", res_tree.tree_id);
//     // printf("tree root id: %d\n", res_tree.root);
//     // for(int i=0; i<res_tree.Nodes.size(); i++){
//     //     exec_node tmp_node = res_tree.Nodes[i];
//     //     printf("node id: %d; \tdata volume: %ld; \t time used: %f.\n", tmp_node.node_id, tmp_node.volume, tmp_node.time_spend);
//     // }
//     // string res_name = "tree_" + to_string(res_tree.tree_id) + "_node_" + to_string(res_tree.root);
//     // my_mysql_res_print(res_name); 

//     /* 以下是测试insert——delete所用代码 */
//     // vector<string> sitenames;
//     // sitenames.push_back("s3");
//     // sitenames.push_back("s4");

//     // vector<string> sqls;
//     // sqls.push_back("insert into publisher_3 values(666666,'myname','PRC')");
//     // sqls.push_back("insert into publisher_4 values(666666,'myname','PRC')");

//     // string res = Data_Insert_Delete_Execute(sitenames, sqls);
//     // printf("%s\n", res.data());

//     // return 0;
// }

/* for循环内原先的内容被封装为另外一个函数，
输入sitenames, sqls, table_names, 输出String - "xx rows imported on site x.\n" 或者 "FAIL on site x.\n" */
void Data_Load_Thread(string site, string frag_sql, string frag_name, std::promise<string> &resultObj){
    /* 执行分片语句 */
    string frag_res;
    int row_num;
    string res_output;
    /* 这里固定s1的意思是固定在每台机子的第一个MySQL实例上执行 */
    /* loacl_name和frag_name其实一模一样，所以爱怎么放怎么放，后面直接用frag_name */
    string local_name = Local_Select(frag_sql, frag_name, "s1");
    /* 获得行数，这个函数要出现在LOAD前面，不然会把原来的表删掉 */
    row_num = my_mysql_res_get_rows(frag_name);
    // printf("local_name: %s", local_name);
    /* 在目标site上执行存储语句 */
    /* 判断一下是否为本地，并执行对应存储函数 */
    if(site == LOCALSITE){
        frag_res = Local_Tmp_Load(frag_name, site);
        // printf("localsite.\n%s\n", frag_res.data());
    }
    else if(site == LOCALSITE2){
        frag_res = Local_Tmp_Load(frag_name, site);
    }
    else{
        // printf("not localsite.");
        frag_res = RPC_Local_Tmp_Load(local_name, site);
        // frag_res = Local_Tmp_Load(frag_name, site); // 先这么写着，防止transfer那边的问题
        // printf("success .");
        // printf("%s", frag_res.data());
        // cout << frag_res;
    }
    // frag_res = "OK";
    if(frag_res == "OK"){ /* 目标site上存储成功 */
        /* 构造输出语句 */
        res_output = to_string(row_num);
        res_output.append(" rows imported on site ");
        res_output.append(site);
        res_output.append(".\n");
        
        // printf("%s\n", res_output.data());
        // return res_output;
    }
    else{
        // printf("FAIL TO LOAD %s", frag_name);
        res_output = "FAIL on site ";
        res_output.append(site);
        res_output.append("\n");
        // printf("%s\n", res_output.data());
        // return res_output_2;
    }
    resultObj.set_value(res_output);
}

string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names){
    /* 将数据存入本地数据库 */
    /* 记录开始时间 */
    time_t start_time = time(NULL);
    int i;
    // vector<thread> load_threads;
    // vector<std::future<string>> result_objs;
    /* 先在本地创建和导入表 */
    /* 这里固定s1的意思是固定在每台机子的第一个MySQL实例上执行 */
    string local_load_res = local_Load(create_sql, load_sql, "s1");
    if(local_load_res == "OK"){ /* 本地导入表成功 */
        /* 执行分片并存储 */
        /* 这里有两种方式，一是遍历执行，二是并行执行，此处想要完成第二种 */
        /* for循环内原先的内容被封装为另外一个函数，输入sitenames, sqls, table_names, 输出String - "xx rows imported on site x.\n" 或者 "FAIL on site x.\n" */
        // string frag_sql;
        // string frag_name;
        // string site;
        /* 先把空间给申请好 */
        std::promise<string> resultObjs[MAXTHREAD]; 
        std::thread load_threads[MAXTHREAD];
        std::future<string> load_sentences[MAXTHREAD];
        // string frag_res[MAXTHREAD];
        // int row_num[MAXTHREAD];
        // string res_output[MAXTHREAD];

        for(i = 0; i < sitenames.size(); i++){
            // frag_sql = sqls[i];
            // frag_name = table_names[i];
            // site = sitenames[i];            
            /* 开启一个分片并在对应site存储的线程，通过传promise类给线程，让线程把结果给future类，实现结果返回 */
            // std::promise<string> resultObj;
            // std::future<string> resultSen = resultObj.get_future();
            load_sentences[i] = resultObjs[i].get_future();
            load_threads[i] = std::thread(Data_Load_Thread, sitenames[i], sqls[i], table_names[i], std::ref(resultObjs[i]));
            // result_objs.push_back(resultObj.get_future());
        }

        /* 从每个进程中获得返回结果并汇总 */
        vector<string> load_results;
        for(i = 0; i < sitenames.size(); i++){
            // std::future<string> resultSent = result_objs[i];
            string load_sentence = load_sentences[i].get();
            load_results.push_back(load_sentence);
        }

        /* 本意是说所有线程完成了主函数才继续，
        但是可能会造成join了第一个线程，就等，等到第一个线程结束了才会join第二个线程？ 
        似乎效果确实一样 */
        for(i = 0; i < sitenames.size(); i++){
            // std::thread th = load_threads[i];
            load_threads[i].join();
        }

        /* 删除本地存储的表 */
        string drop_sql = "drop table ";
        drop_sql.append(main_name);
        string local_release_res = local_Insert_Delete(drop_sql, "s1");
        if (local_release_res == "FAILED") 
        {   /*现在就代表执行失败了*/
            printf("Error： mysql release error!\n");
        }
        else 
        { 
            /*现在就代表执行成功了*/
        }
        /* 计算所花时间 */
        time_t end_time = time(NULL);
        double time_spend = difftime(end_time, start_time);
        /* 构造输出语句 */
        string time_output = to_string(time_spend);
        time_output.append(" seconds used.\n");
        string output_sentence = "";
        for(i = 0; i < load_results.size(); i++){
            output_sentence.append(load_results[i]);
        }
        output_sentence.append(time_output);
        return output_sentence;
        

    }
    else{
        printf("LOCAL LOAD FAILED!");
        return "FAIL";
    }
}

/* 把int型的节点名称（1）转化为字符串型（"s1"） */
string site_to_string(int site_num){
    string site = "s";
    string num = to_string(site_num);
    site.append(num);
    return site;
}

/* 通过系统stat结构体获取文件大小，单位bytes，size_t为长整型，若要打印占位符为%ld */
size_t get_filebytes(const char *filename)
{
   size_t flag;
   size_t file_size=0;
   struct stat statbuf; 
   flag=stat(filename,&statbuf); //使用stat()获取文件信息
   if(flag!=0)
	   printf("Get file information error\n");
   else
   {
           printf("Get file information success\n");
           file_size=statbuf.st_size; //获取文件大小
   }
   return file_size;
}

/* 输入一个计划树结构，返回这棵树的根节点 */
NODE get_root(TREE tree){
    int root_id = tree.root;
    int i;
    NODE root_node;
    root_node.id = -1;
    for(i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].id == root_id){
            root_node = tree.Nodes[i];
            return root_node;
        }
    }
    if(root_node.id == -1){
        printf("root node does not exist!");
    }
}

/* 输入一个执行树结构，返回这棵树的根节点 */
exec_node get_root(exec_tree tree){
    int root_id = tree.root;
    int i;
    exec_node root_node;
    root_node.node_id = -1;
    for(i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].node_id == root_id){
            root_node = tree.Nodes[i];
            return root_node;
        }
    }
    if(root_node.node_id == -1){
        printf("root node does not exist!");
    }
}

/* 输入一个树结构和节点id，返回对应节点 */
NODE get_node(TREE tree, int node_id){
    int i;
    NODE target_node;
    target_node.id = -1;
    for(i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].id == node_id){
            target_node = tree.Nodes[i];
            return target_node;
        }
    }
    if(target_node.id == -1){
        printf("target node does not exist!");
    }
}

/* 输入一个树结构和节点id，返回以对应节点为根节点的子树，也是个递归函数 */
TREE get_sub_tree(TREE tree, int node_id){
    TREE sub_tree;
    sub_tree.tree_id = tree.tree_id;
    sub_tree.root = -1;
    vector<NODE> sub_nodes;
    /* 判断是否是叶子节点 */
    for(int i=0; i<tree.Nodes.size(); i++){
        if(tree.Nodes[i].id == node_id){
            sub_tree.root = node_id;
            NODE tmp_node = tree.Nodes[i];
            sub_nodes.push_back(tmp_node);
            /* 如果是叶子节点，就只放根节点进去然后返回 */
            if(tree.Nodes[i].child.size() == 0){
                // printf("leaf node id %d\n", tree.Nodes[i].id);
                sub_tree.Nodes = sub_nodes;
                return sub_tree;
            }
            else{
                /* 如果不是叶子节点，就找它的孩子代表的子树，合并 */
                vector<int> childs_id = tree.Nodes[i].child;
                for(int k=0; k<childs_id.size(); k++){
                    TREE child_sub_tree = get_sub_tree(tree, childs_id[k]);
                    /* 把子树的节点都加入本树的节点 */
                    for(int j=0; j<child_sub_tree.Nodes.size(); j++){
                        sub_nodes.push_back(child_sub_tree.Nodes[j]);
                    }
                }
                /* 去除重复的，只是为了验证别的地方对不对，真正的树应当没有重复的内容 */
                vector<NODE> sub_nodes_unique;
                int flag = 0;
                for(NODE tmp_node: sub_nodes){
                    flag = 0;
                    for(NODE tmp_node_unique: sub_nodes_unique){
                        if(tmp_node.id == tmp_node_unique.id){
                            /* 之前出现过 */
                            flag = 1;
                            break;
                        }
                    }
                    if(flag == 0){
                        /* 之前没出现过，加入列表 */
                        sub_nodes_unique.push_back(tmp_node);
                    }
                }
                sub_tree.Nodes = sub_nodes_unique;
                return sub_tree;
            }
            
        }      
    }
    if(sub_tree.root == -1){
        printf("this node does not exist!\n");
        sub_tree.Nodes = sub_nodes;
        return sub_tree;
    }
}

/* RPC_Data_Select_Execute的可并行化版本，合并前先注释掉方便调试 */
void RPC_Data_Select_Execute_Thread(TREE tree, string site, std::promise<exec_tree> &resultObj){
    /* RPC_Data_Select_Execute 只会调用Data_Select_Execute */
    exec_tree res_sub_tree = RPC_Data_Select_Execute(tree, site);
    /* 把文件传过来 */
    string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(tree.root);
    string transfer_res = RPC_GET_FILE(res_name, site);
    if(transfer_res=="OK"){
        printf("%s transfered success!\n", res_name);
    }
    resultObj.set_value(res_sub_tree);
}

/* Data_Select_Execute的可并行化版本，这里面只会调用可并行化版本的Select_Execute（自身或者RPC_Data_Select_Execute_Thread） */
void Data_Select_Execute_Thread(TREE tree, std::promise<exec_tree> &resultObj){
    TREE sub_tree;
    exec_tree res_tree;
    NODE child_node;
    res_tree.tree_id = tree.tree_id;
    res_tree.root = tree.root;
    /* 获得开始时间 */
    time_t start_time = time(NULL);
    /* 读计划树的根节点，把相同部分先放进执行树根节点 */
    NODE root_node = get_root(tree);
    vector<int> childs_id = root_node.child;
    exec_node exec_root_node;
    exec_root_node.node_id = root_node.id;
    exec_root_node.site = root_node.site;
    exec_root_node.child = root_node.child;
    exec_root_node.parent = root_node.parent;
    /* 判断是否叶子节点 */
    if(tree.Nodes.size()==1){
        /* 是叶子节点，从MySQL里面具体拿数据 */
        string site = site_to_string(root_node.site);
        string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(root_node.id);
        string sql_statement = root_node.sql_statement;
        string select_res;
        if(site == LOCALSITE || site == LOCALSITE2){
            select_res = Local_Select(sql_statement, res_name, site);
        }
        else{
            // select_res = RPC_Local_Select(sql_statement, res_name, site);
            select_res = Local_Select(sql_statement, res_name, site); // 本地调试用，我觉得这个分支可能永远也进入不了吧
        }
        /* 获取结果文件大小 */
        if(select_res == "FAIL"){
            /* 如果查询失败了 */
            /* 计算所花时间 */
            time_t end_time = time(NULL);
            double time_spend = difftime(end_time, start_time);
            /* 构造本节点的执行记录，合并进执行树 */
            exec_root_node.time_spend = time_spend;
            exec_root_node.volume = 0;
            exec_root_node.res = "FAIL";
            res_tree.Nodes.push_back(exec_root_node);
            resultObj.set_value(res_tree);
            return;
            // return res_tree;
        }
        else{
            /* 现在表示查询成功了 */
            string filepath = TMPPATH + res_name + ".sql";
            const char* p = filepath.data();
            size_t res_volume = get_filebytes(p);
            /* 计算所花时间 */
            time_t end_time = time(NULL);
            double time_spend = difftime(end_time, start_time);
            /* 构造本节点的执行记录，合并进执行树 */
            exec_root_node.time_spend = time_spend;
            exec_root_node.volume = res_volume;
            exec_root_node.res = "OK";
            res_tree.Nodes.push_back(exec_root_node);
            resultObj.set_value(res_tree);
            return;
            // return res_tree;
        }
    }
    else{
        /* 不是叶子节点 */
        /* 遍历根节点的子节点，获得以该子节点为根节点的子树，这里的遍历先写成循环，通顺以后再改成多线程  */
        /* 先把空间给申请好 */
        std::promise<exec_tree> resultObjs[MAXTHREAD]; 
        std::thread select_threads[MAXTHREAD];
        std::future<exec_tree> select_thread_exec_trees[MAXTHREAD];
        int q = 0; // 计数
        for(q=0; q<childs_id.size(); q++){
            printf("child %d on processing ...\n", childs_id[q]);
            sub_tree = get_sub_tree(tree, childs_id[q]);
            child_node = get_node(tree, childs_id[q]);
            string site = site_to_string(child_node.site);
            // string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(child_node.id);

            /* 继续执行本函数，得到返回的执行树 */
            // exec_tree res_sub_tree;
            select_thread_exec_trees[q] = resultObjs[q].get_future();
            
            if(site == LOCALSITE || site == LOCALSITE2){
                // res_sub_tree = Data_Select_Execute(sub_tree);
                select_threads[q] = std::thread(Data_Select_Execute_Thread, sub_tree, std::ref(resultObjs[q]));
            }
            else{
                // res_sub_tree = RPC_Data_Select_Execute(sub_tree, site);
                // res_sub_tree = Data_Select_Execute(sub_tree); // 本地调试用
                select_threads[q] = std::thread(RPC_Data_Select_Execute_Thread, sub_tree, site, std::ref(resultObjs[q]));
                // select_threads[q] = std::thread(Data_Select_Execute_Thread, sub_tree, std::ref(resultObjs[q])); // 本地调试用
            }
            /* 线程函数到此为止 */
            // q = q + 1;
        }
        
        /* 从每个进程中获得返回结果并处理 */
        for(q=0; q<childs_id.size(); q++){
            string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(childs_id[q]);
            exec_tree res_sub_tree = select_thread_exec_trees[q].get();
            exec_node res_sub_root = get_root(res_sub_tree);
            if(res_sub_root.res=="OK"){
                /* 子树执行成功，把节点都加入执行树节点 */ 
                for(exec_node tmp_exec_node: res_sub_tree.Nodes){
                    res_tree.Nodes.push_back(tmp_exec_node);
                }
                /*把结果存进本地MySQL，通用s1 */               
                string tmp_load_res = Local_Tmp_Load(res_name, "s1");
                if(tmp_load_res == "OK"){
                    printf("tmp table loading of %s succeed!\n", res_name.data());
                }
                else{
                    /* 计算所花时间 */
                    time_t end_time = time(NULL);
                    double time_spend = difftime(end_time, start_time);
                    /* 构造本节点的执行记录，合并进执行树 */
                    exec_root_node.time_spend = time_spend;
                    exec_root_node.volume = 0;
                    exec_root_node.res = "FAIL";
                    res_tree.Nodes.push_back(exec_root_node);
                    resultObj.set_value(res_tree);
                    return;
                    // return res_tree;
                }
            }
            else{
                /* 子树执行失败 */
                /* 计算所花时间 */
                time_t end_time = time(NULL);
                double time_spend = difftime(end_time, start_time);
                /* 构造本节点的执行记录，合并进执行树 */
                exec_root_node.time_spend = time_spend;
                exec_root_node.volume = 0;
                exec_root_node.res = "FAIL";
                res_tree.Nodes.push_back(exec_root_node);
                resultObj.set_value(res_tree);
                return;
                // return res_tree;
            }

        }
        /* 利用join让所有线程完成之后主函数再继续 */
        for(q=0; q<childs_id.size(); q++){
            select_threads[q].join();
        }
    }
    /* 根据根节点的sql_statement执行结果整合 */
    string root_sql = root_node.sql_statement;
    string root_res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(root_node.id);
    string root_site = site_to_string(root_node.site);
    string root_select_res = Local_Select(root_sql, root_res_name, root_site);

    /* 删除MySQL内部临时表，通用s1，中间失败的话不影响结果，只是长此以往会造成数据库负担，所以没有因此而返回FAIL */
    for(int child_id: childs_id){
        string drop_sql = "drop table tree_" + to_string(tree.tree_id) + "_node_" + to_string(child_id);
        string drop_res = local_Insert_Delete(drop_sql, "s1");
        if(drop_res != "OK"){
            printf("release failed! %s", drop_sql.data());
        }
    }

    if(root_select_res == "FAIL"){
        /* 如果整合查询失败了 */
        /* 计算所花时间 */
        time_t end_time = time(NULL);
        double time_spend = difftime(end_time, start_time);
        /* 构造本节点的执行记录，合并进执行树 */
        exec_root_node.time_spend = time_spend;
        exec_root_node.volume = 0;
        exec_root_node.res = "FAIL";
        res_tree.Nodes.push_back(exec_root_node);
        resultObj.set_value(res_tree);
        return;
        // return res_tree;
    }
    else{
        /* 现在表示整合查询成功了 */
        /* 获取结果文件大小 */
        string root_filepath = TMPPATH + root_res_name + ".sql";
        const char* root_p = root_filepath.data();
        size_t root_res_volume = get_filebytes(root_p);

        /* 计算所花时间 */
        time_t end_time = time(NULL);
        double time_spend = difftime(end_time, start_time);
        /* 构造本节点的执行记录，合并进执行树 */
        exec_root_node.time_spend = time_spend;
        exec_root_node.volume = root_res_volume;
        exec_root_node.res = "OK";
        res_tree.Nodes.push_back(exec_root_node);
        resultObj.set_value(res_tree);
        return;
        // return res_tree;
    }
}

exec_tree Data_Select_Execute(TREE tree){
    TREE sub_tree;
    exec_tree res_tree;
    NODE child_node;
    res_tree.tree_id = tree.tree_id;
    res_tree.root = tree.root;
    /* 获得开始时间 */
    time_t start_time = time(NULL);
    /* 读计划树的根节点，把相同部分先放进执行树根节点 */
    NODE root_node = get_root(tree);
    vector<int> childs_id = root_node.child;
    exec_node exec_root_node;
    exec_root_node.node_id = root_node.id;
    exec_root_node.site = root_node.site;
    exec_root_node.child = root_node.child;
    exec_root_node.parent = root_node.parent;
    /* 判断是否叶子节点 */
    if(tree.Nodes.size()==1){
        /* 是叶子节点，从MySQL里面具体拿数据 */
        string site = site_to_string(root_node.site);
        string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(root_node.id);
        string sql_statement = root_node.sql_statement;
        string select_res;
        if(site == LOCALSITE || site == LOCALSITE2){
            select_res = Local_Select(sql_statement, res_name, site);
        }
        else{
            // select_res = RPC_Local_Select(sql_statement, res_name, site);
            select_res = Local_Select(sql_statement, res_name, site); // 本地调试用，我觉得这个分支可能永远也进入不了吧
        }
        printf("select_res: %s\n", select_res.data());
        /* 获取结果文件大小 */
        if(select_res == "FAIL"){
            /* 如果查询失败了 */
            /* 计算所花时间 */
            time_t end_time = time(NULL);
            double time_spend = difftime(end_time, start_time);
            /* 构造本节点的执行记录，合并进执行树 */
            exec_root_node.time_spend = time_spend;
            exec_root_node.volume = 0;
            exec_root_node.res = "FAIL";
            res_tree.Nodes.push_back(exec_root_node);
            return res_tree;
        }
        else{
            /* 现在表示查询成功了 */
            string filepath = TMPPATH + res_n  ame + ".sql";
            const char* p = filepath.data();
            size_t res_volume = get_filebytes(p);
            /* 计算所花时间 */
            time_t end_time = time(NULL);
            double time_spend = difftime(end_time, start_time);
            /* 构造本节点的执行记录，合并进执行树 */
            exec_root_node.time_spend = time_spend;
            exec_root_node.volume = res_volume;
            exec_root_node.res = "OK";
            res_tree.Nodes.push_back(exec_root_node);
            return res_tree;
        }
    }
    else{
        /* 不是叶子节点 */
        /* 遍历根节点的子节点，获得以该子节点为根节点的子树，这里的遍历先写成循环，通顺以后再改成多线程  */
        /* 先把空间给申请好 */
        std::promise<exec_tree> resultObjs[MAXTHREAD]; 
        std::thread select_threads[MAXTHREAD];
        std::future<exec_tree> select_thread_exec_trees[MAXTHREAD];
        int q = 0; // 计数
        for(q=0; q<childs_id.size(); q++){
            printf("child %d on processing ...\n", childs_id[q]);
            sub_tree = get_sub_tree(tree, childs_id[q]);
            child_node = get_node(tree, childs_id[q]);
            string site = site_to_string(child_node.site);
            // string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(child_node.id);

            /* 继续执行本函数，得到返回的执行树 */
            // exec_tree res_sub_tree;
            select_thread_exec_trees[q] = resultObjs[q].get_future();
            
            if(site == LOCALSITE || site == LOCALSITE2){
                // res_sub_tree = Data_Select_Execute(sub_tree);
                select_threads[q] = std::thread(Data_Select_Execute_Thread, sub_tree, std::ref(resultObjs[q]));
            }
            else{
                // res_sub_tree = RPC_Data_Select_Execute(sub_tree, site);
                // res_sub_tree = Data_Select_Execute(sub_tree); // 本地调试用
                select_threads[q] = std::thread(RPC_Data_Select_Execute_Thread, sub_tree, site, std::ref(resultObjs[q]));
                // select_threads[q] = std::thread(Data_Select_Execute_Thread, sub_tree, std::ref(resultObjs[q])); // 本地调试用
            }
            /* 线程函数到此为止 */
            // q = q + 1;
        }
        
        /* 从每个进程中获得返回结果并处理 */
        for(q=0; q<childs_id.size(); q++){
            string res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(childs_id[q]);
            exec_tree res_sub_tree = select_thread_exec_trees[q].get();
            exec_node res_sub_root = get_root(res_sub_tree);
            if(res_sub_root.res=="OK"){
                /* 子树执行成功，把节点都加入执行树节点 */ 
                for(exec_node tmp_exec_node: res_sub_tree.Nodes){
                    res_tree.Nodes.push_back(tmp_exec_node);
                }
                /*把结果存进本地MySQL，通用s1 */               
                string tmp_load_res = Local_Tmp_Load(res_name, "s1");
                if(tmp_load_res == "OK"){
                    printf("tmp table loading of %s succeed!\n", res_name.data());
                }
                else{
                    /* 计算所花时间 */
                    time_t end_time = time(NULL);
                    double time_spend = difftime(end_time, start_time);
                    /* 构造本节点的执行记录，合并进执行树 */
                    exec_root_node.time_spend = time_spend;
                    exec_root_node.volume = 0;
                    exec_root_node.res = "FAIL";
                    res_tree.Nodes.push_back(exec_root_node);
                    return res_tree;
                }
            }
            else{
                /* 子树执行失败 */
                /* 计算所花时间 */
                time_t end_time = time(NULL);
                double time_spend = difftime(end_time, start_time);
                /* 构造本节点的执行记录，合并进执行树 */
                exec_root_node.time_spend = time_spend;
                exec_root_node.volume = 0;
                exec_root_node.res = "FAIL";
                res_tree.Nodes.push_back(exec_root_node);
                return res_tree;
            }

        }
        /* 利用join让所有线程完成之后主函数再继续 */
        for(q=0; q<childs_id.size(); q++){
            select_threads[q].join();
        }
    }
    /* 根据根节点的sql_statement执行结果整合 */
    string root_sql = root_node.sql_statement;
    string root_res_name = "tree_" + to_string(tree.tree_id) + "_node_" + to_string(root_node.id);
    string root_site = site_to_string(root_node.site);
    string root_select_res = Local_Select(root_sql, root_res_name, root_site);

    /* 删除MySQL内部临时表，通用s1，中间失败的话不影响结果，只是长此以往会造成数据库负担，所以没有因此而返回FAIL */
    for(int child_id: childs_id){
        string drop_sql = "drop table tree_" + to_string(tree.tree_id) + "_node_" + to_string(child_id);
        string drop_res = local_Insert_Delete(drop_sql, "s1");
        if(drop_res != "OK"){
            printf("release failed! %s", drop_sql.data());
        }
    }

    if(root_select_res == "FAIL"){
        /* 如果整合查询失败了 */
        /* 计算所花时间 */
        time_t end_time = time(NULL);
        double time_spend = difftime(end_time, start_time);
        /* 构造本节点的执行记录，合并进执行树 */
        exec_root_node.time_spend = time_spend;
        exec_root_node.volume = 0;
        exec_root_node.res = "FAIL";
        res_tree.Nodes.push_back(exec_root_node);
        return res_tree;
    }
    else{
        /* 现在表示整合查询成功了 */
        /* 获取结果文件大小 */
        string root_filepath = TMPPATH + root_res_name + ".sql";
        const char* root_p = root_filepath.data();
        size_t root_res_volume = get_filebytes(root_p);

        /* 计算所花时间 */
        time_t end_time = time(NULL);
        double time_spend = difftime(end_time, start_time);
        /* 构造本节点的执行记录，合并进执行树 */
        exec_root_node.time_spend = time_spend;
        exec_root_node.volume = root_res_volume;
        exec_root_node.res = "OK";
        res_tree.Nodes.push_back(exec_root_node);
        return res_tree;
    }
}

/* for循环内原先的内容被封装为另外一个函数，
输入sitenames, sqls, 输出String - "OK on site x.\n" 或者 "FAIL on site x.\n" 
最后三个参数是为了传递主函数给每个线程的对应变量预留的空间，而不是传递值 */
void Data_Insert_Delete_Thread(string site, string frag_sql, std::promise<string> &resultObj){
    /* 判断一下是否为本地，并执行对应sql语句 */
    string frag_res;
    string res_output;
    if(site == LOCALSITE || site == LOCALSITE2){
        frag_res = local_Insert_Delete(frag_sql, site);
        // printf("localsite.\n%s\n", frag_res.data());
    }
    else{
        // printf("not localsite.");
        frag_res = RPC_local_Insert_Delete(frag_sql, site);
        // frag_res = Local_Tmp_Load(frag_name, site); // 先这么写着，防止transfer那边的问题
        // printf("success .");
        // printf("%s", frag_res.data());
        // cout << frag_res;
    }
    // frag_res = "OK";
    if(frag_res == "OK"){ /* 目标site上存储成功 */
        /* 构造输出语句 */
        res_output = "OK on site ";
        res_output.append(site);
        res_output.append(".\n");
        
        // printf("%s\n", res_output.data());
        // return res_output;
    }
    else{
        // printf("FAIL TO LOAD %s", frag_name);
        res_output = "FAIL on site ";
        res_output.append(site);
        res_output.append(".\n");
        // printf("%s\n", res_output.data());
        // return res_output_2;
    }
    resultObj.set_value(res_output);
}

/* 本函数供parser调用
本函数用于执行整个Insert或Delete流程，输入站点列表，分片sql语句列表，返回
"OK/FAIL on site 1.
 OK/FAIL on site 2.
 y seconds used." */
string Data_Insert_Delete_Execute(vector<string> sitenames, vector<string> sqls){
    /* 记录开始时间 */
    time_t start_time = time(NULL);
    int i;
    /* 先把空间给申请好 */
    std::promise<string> resultObjs[MAXTHREAD]; 
    std::thread load_threads[MAXTHREAD];
    std::future<string> load_sentences[MAXTHREAD];

    for(i = 0; i < sitenames.size(); i++){           
        /* 开启一个分片并在对应site存储的线程，通过传promise类给线程，让线程把结果给future类，实现结果返回 */
        load_sentences[i] = resultObjs[i].get_future();
        load_threads[i] = std::thread(Data_Insert_Delete_Thread, sitenames[i], sqls[i], std::ref(resultObjs[i]));
    }

    /* 从每个进程中获得返回结果并汇总 */
    vector<string> load_results;
    for(i = 0; i < sitenames.size(); i++){
        string load_sentence = load_sentences[i].get();
        load_results.push_back(load_sentence);
    }

    /* 本意是说所有线程完成了主函数才继续 */
    for(i = 0; i < sitenames.size(); i++){
        load_threads[i].join();
    }
    /* 计算所花时间 */
    time_t end_time = time(NULL);
    double time_spend = difftime(end_time, start_time);
    /* 构造输出语句 */
    string time_output = to_string(time_spend);
    time_output.append(" seconds used.\n");
    string output_sentence = "";
    for(i = 0; i < load_results.size(); i++){
        output_sentence.append(load_results[i]);
    }
    output_sentence.append(time_output);
    return output_sentence;
}