#include <stdio.h>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <typeinfo> /* 为了调试 */
// #include "../../metadata/metadata.h"

using namespace std;

#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "rootroot"
#define DATABASE "test"  // here the name should be changed to our databases
// #define PORT 7654  // here the site1, it can be changed to 7655 for site2
// #define UNIX_SOCKET "/home/mysql1/mysql.sock" // here the site1, later can be changed to "home/mysql2/mysql.sock" for site2
#define TMPPATH "/mnt/d/ddbms200tmp/"


// // 元信息结构定义
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

// mysql自己定义了查询结果的结构
// #define MAX_COLUMN=10;
// typedef struct my_mysql_res {
//     string global_table;                 // 结果集来源的表名
//     int row_count;                       // 结果集的行数
//     unsigned int field_count;            // 结果集的列数
//     MY_MYSQL_COL col[MAX_COLUMN];        // 结果集的数据，按列存储
// } MY_MYSQL_RES; // 结果集

// typedef struct st_mysql_field {
//     string name;                         // 列名
//     string values;                       // 列值，"\t"分隔
// }  MY_MYSQL_COL; // 结果集的列

// 取自mysql自己定义的查询结果的结构
// typedef struct st_mysql_res {
//   my_ulonglong row_count;                               // 结果集的行数
//   unsigned int field_count, current_field;            // 结果集的列数，当前列
//   MYSQL_FIELD *fields;                                    // 结果集的列信息
//   MYSQL_DATA *data;                                      // 结果集的数据
//   MYSQL_ROWS *data_cursor;                        // 结果集的光标
//   MEM_ROOT field_alloc;                                   // 内存结构
//   MYSQL_ROW row;                                         // 非缓冲的时候用到
//   MYSQL_ROW current_row;                           //mysql_store_result时会用到。当前行
//   unsigned long *lengths;                                //每列的长度
//   MYSQL *handle;                                           // mysql_use_result会用。
//   my_bool eof;                                                 //是否为行尾
// } MYSQL_RES;
// typedef char ** MYSQL_ROW; /* 返回的每一行的值，全部用字符串来表示*/
// typedef struct st_mysql_rows {
//   struct st_mysql_rows *next; /* list of rows */
//   MYSQL_ROW data;
// }  MYSQL_ROWS;        //mysql的数据的链表节点。可见mysql的结果集是链表结构
// typedef struct st_mysql_data {
//   my_ulonglong rows;
//   unsigned int fields;
//   MYSQL_ROWS *data;
//   MEM_ROOT alloc;
// }  MYSQL_DATA; // 数据集的结构
// typedef struct st_mysql_field {
//   char *name;  /* Name of column */
//   char *table;                                            /* Table of column if column was a field */
//   char *def;                                               /* Default value (set by mysql_list_fields) */
//   enum enum_field_types type;                /* Type of field. Se mysql_com.h for types */
//   unsigned int length;                               /* Width of column */
//   unsigned int max_length;                       /* Max width of selected set */
//   unsigned int flags;                                  /* Div flags */
//   unsigned int decimals;                            /* Number of decimals in field */
// }  MYSQL_FIELD;  //列信息的结构
// typedef struct st_used_mem { /* struct for once_alloc */
//   struct st_used_mem *next; /* Next block in use */
//   unsigned int left; /* memory left in block  */
//   unsigned int size; /* Size of block */
// } USED_MEM; //内存结构
// typedef struct st_mem_root {
//   USED_MEM *free;
//   USED_MEM *used;
//   USED_MEM *pre_alloc;
//   unsigned int min_malloc;
//   unsigned int block_size;
//   void (*error_handler)(void);
// } MEM_ROOT;  //内存结构


// 目前我自己定义的返回结果的形式 
// - 现在改成sql文件形式传输了，要求是每个查询树的每个节点所代表的数据表名称（编号）唯一
	

/* 本地执行插入和删除函数，输入SQL语句和站点名称（s1, s2, s3, s4），返回执行结果(OK or FAILED) */
string local_Insert_Delete(string sql, string site);

/* 本地执行文件导入函数，输入创建表的SQL语句和导入文件的SQL语句，和站点名称（s1, s2, s3, s4）,返回执行结果(OK or FAIL)
   例子：以Book表为例
   local_Load("create table book(id int(6), title char(100), authors char(200), publisher_id int(6), copies int(5), key(id) )", "load data local infile '/home/roy/ddbms/rawdata/book.tsv' into table book"); */
string local_Load(string sql_create, string sql_load, string site);

/* 本地执行查询函数，输入SQL语句，和指定返回结果集名称（唯一），和站点名称（s1, s2, s3, s4）,返回执行结果表名(全局唯一), 或者"FAIL"(暂时没有了) */
string Local_Select(string sql, string res_name, string site);

/* 本地执行表存储函数，输入待存的数据表名，和站点名称（s1, s2, s3, s4）,返回执行结果(OK or FAIL) */
string Local_Tmp_Load(string tmp_data, string site);

/* 相当于打印sql文件里的数据
   输出样例为：
查询到 2 行 
id      title   authors publisher_id    copies
200001  Book #200001    H. Johnston     100366  7231
200002  Book #200002    L. Houghton     101543  694 */
void my_mysql_res_print(string my_res);

/* 相当于输出sql文件里的数据到某个文件，输入sql文件名和输出文件名（带后缀），例如"output.txt"
   输出样例为： 
id      title          authors         publisher_id    copies
200001  Book#200001    H. Johnston     100366          7231
200002  Book#200002    L. Houghton     101543          694 */
void my_mysql_res_output(string my_res, string filename);

/* 获得SQL文件里数据的行数，要在load之前（之后再也不需要数据库里有这个数据）  */
int my_mysql_res_get_rows(string my_res);

/* 获得SQL文件里第一列数据(此处其实特意为id而设)，要在load之前（之后再也不需要数据库里有这个数据）  */
vector<int> my_mysql_res_get_column1(string my_res);