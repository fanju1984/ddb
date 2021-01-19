#include "./executor.h"
// #include <iostream>
// #include <string>
// #include <vector>
// #include <ctime>
// #include "./mysql_connector.h"
// #include "../../transfer/transfer/transfer.h"

// /* 在每个site实例上部署的时候这个值不同 */
// #define LOCALSITE "s1"


// // 现在定义一下时间和数据传输量记录的结构
// // 也用树结构记录每个节点花费的时间和数据传输量
// struct exec_node{
//     int node_id; // 原来树中该节点对应的ID
//     double time_spend; // 执行对应节点所花的时间，单位为秒
//     int volume; // 该节点上结果的数据量，单位为比特
// };
// struct exec_tree{
//     int tree_id; // 应当与它执行的树的ID一致
//     vector<exec_node> nodes; // 应当与它执行的树的node数量一致
// };


// // string Data_Load_Execute(string create_sql, string load_sql, vector<string> sitenames, vector<string> sqls, vector<string> table_names){
// //     return "x rows imported, y seconds used."
// // }
// // 这里是一条一条给我的？

// /* 本函数用于执行整个load流程，输入本地创建和导入表的SQL语句，本地表的表名，站点列表，
// 分片select语句列表和分片表名列表，返回
// "x1 rows imported on site 1.
//  x2 rows imported on site2.
//  y seconds used."
//  或者"FAIL" */
// string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names);

// int main(int argc,char *argv[])
// {
//     /* id int key, name char(100), nation char(3) */
//     string create_sql = "create table publisher(id int(6), name char(100), nation char(3))";
//     string load_sql = "load data local infile '/home/roy/ddbms/rawdata/publisher.tsv' into table publisher";
//     string main_name = "pulisher";
//     vector<string> sitenames;
//     string site = "s2";
//     sitenames.push_back(site);
//     vector<string> sqls;
//     string sql = "select * from publisher where id < 104000 and nation='PRC'";
//     sqls.push_back(sql);
//     vector<string> table_names;
//     string table_name = "publisher_1";
//     table_names.push_back(table_name);

//     string load_output = Data_Load_Execute(create_sql, load_sql, main_name, sitenames, sqls, table_names);
//     printf("%s", load_output.data());
//     return 0;
// }

string Data_Load_Execute(string create_sql, string load_sql, string main_name, vector<string> sitenames, vector<string> sqls, vector<string> table_names){
    /* 将数据存入本地数据库 */
    /* 记录开始时间 */
    time_t start_time = time(NULL);
    int i;
    int row_num;
    vector<string> load_results;
    /* 先在本地创建和导入表 */
    /* 这里固定s1的意思是固定在每台机子的第一个MySQL实例上执行 */
    string local_load_res = local_Load(create_sql, load_sql, "s1");
    if(local_load_res == "OK"){ /* 本地导入表成功 */
        /* 执行分片并存储 */
        /* 这里有两种方式，一是遍历执行，二是并行执行，此处先完成第一种 */
        for(i = 0; i < sitenames.size(); i++){                                            
            /* 执行分片语句 */
            /* 这里固定s1的意思是固定在每台机子的第一个MySQL实例上执行 */
            string frag_sql = sqls[i];
            string frag_name = table_names[i];
            string local_name = Local_Select(frag_sql, frag_name, "s1");
            // printf("local_name: %s", local_name);
            /* 在目标site上执行存储语句 */
            string site = sitenames[i];
            string frag_res;
            /* 判断一下是否为本地，并执行对应存储函数 */
            if(site == LOCALSITE){
                // printf("localsite.");
                frag_res = Local_Tmp_Load(local_name, site);
            }
            else if(site == LOCALSITE2){
                frag_res = Local_Tmp_Load(local_name, site);
            }
            else{
                // printf("not localsite.");
                frag_res = RPC_Local_Tmp_Load(local_name, site);
                // printf("success .");
                // printf("%s", frag_res.data());
                cout << frag_res;
            }
            // frag_res = "OK";
            if(frag_res == "OK"){ /* 目标site上存储成功 */
                /* 获得行数 */
                row_num = my_mysql_res_get_rows(local_name);
                /* 构造输出语句 */
                string res_output = to_string(row_num);
                res_output.append(" rows imported on site ");
                res_output.append(site);
                res_output.append(".\n");
                load_results.push_back(res_output);
                continue;
            }
            else{
                printf("FAIL TO LOAD %s", frag_name);
                /* 删除本地存储的表 */
                string drop_sql = "drop table ";
                drop_sql.append(main_name);
                string local_release_res = local_Insert_Delete(drop_sql, "s1");
                if (local_release_res == "FAIL") 
                {   /*现在就代表执行失败了*/
                    printf("Error： mysql release error!\n");
                }
                else 
                { 
                    /*现在就代表执行成功了*/
                }
                return "FAIL";
            }
        }
        if(i == sitenames.size()){
            /* 全部存储成功 */
            /* 删除本地存储的表 */
            string drop_sql = "drop table ";
            drop_sql.append(main_name);
            string local_release_res = local_Insert_Delete(drop_sql, "s1");
            if (local_release_res == "FAIL") 
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
        

    }
    else{
        printf("LOCAL LOAD FAILED!");
        return "FAIL";
    }
}

