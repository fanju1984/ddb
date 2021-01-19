#include "./mysql_connector.h"

// int main(int argc,char *argv[])
// {
//     // string res_str_out;
//     // res_str_out = local_Insert_Delete("delete from test where name='user'");
//     // insert into test values('user','123456')
//     // res_str_out = local_Insert_Delete("insert into test values('user','123456')");
//     // res_str_out = local_Load("create table book(id int(6), title char(100), authors char(200), publisher_id int(6), copies int(5), key(id) )", "load data local infile '/home/roy/ddbms/rawdata/book.tsv' into table book");
//     // const char* p = res_str_out.data();
//     // printf("%s\n", p);
//     /* 从s3读出数据并在s4存成临时表 */
//     // string res_data_out = Local_Select("select * from book where id<=200005", "tmp_book_1", "s3");
//     // const char* p = res_data_out.data();
//     // printf("tmp table name:%s\n", p);
//     // string res_tmp_out = Local_Tmp_Load(res_data_out, "s4");
//     // p = res_tmp_out.data();
//     // printf("tmp table stored on s4: %s\n", p);
//     /* 把临时表查出来并且打印 */
//     string res_data_out = Local_Select("select * from tmp_book_1", "tmp_book_2", "s4");
//     vector<int> data_out = my_mysql_res_get_column1(res_data_out);
//     for(int i=0; i<data_out.size(); i++){
//         printf("%d\n", data_out[i]);
//     }
//     // my_mysql_res_print(res_data_out);

//     return 0;
// }

string local_Insert_Delete(string sql, string site) // 即只返回成功与否的所有sql语句都可以用这个，测试完毕
{
    MYSQL conn;
    int res;

    int PORT;
    const char* UNIX_SOCKET;

    /* 通过站点名称判断使用哪个连接 */
    if(site == "s4"){
        PORT = 7655;
        UNIX_SOCKET = "/home/mysql2/mysql.sock";
    }
    else{
        PORT = 7654;
        UNIX_SOCKET = "/home/mysql1/mysql.sock";
    } // 此处如果输入了没出现过的站点应当报警但我没写

    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        const char* p = sql.data(); 
        res=mysql_query(&conn,p);
        mysql_close(&conn);
        if(res)
        {
            // printf("error\n");
            return "FAILED";
        }
        else
        {
            // printf("OK\n");
            return "OK";
        }
        
    }   
}

string local_Load(string sql_create, string sql_load, string site)
{
    MYSQL conn;
    int res;
    int res_load;

    int PORT;
    const char* UNIX_SOCKET;

    /* 通过站点名称判断使用哪个连接 */
    if(site == "s4"){
        PORT = 7655;
        UNIX_SOCKET = "/home/mysql2/mysql.sock";
    }
    else{
        PORT = 7654;
        UNIX_SOCKET = "/home/mysql1/mysql.sock";
    } // 此处如果输入了没出现过的站点应当报警但我没写


    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        const char* p = sql_create.data(); 
        res=mysql_query(&conn,p);
        if(res) /*创建数据表失败*/
        {
            // printf("error\n");
            mysql_close(&conn);
            return "FAILED";
        }
        else /*创建数据表成功*/
        {
            // printf("OK\n");
            p = sql_load.data();
            res_load = mysql_query(&conn,p);
            mysql_close(&conn);
            if(res_load) /*导入数据失败*/
            {
                return "FAILED";
            }
            else{
                return "OK";
            }
            
        }
        
    }       
}

string Local_Tmp_Load(string tmp_data, string site)
{
    int PORT;
    const char* UNIX_SOCKET;

    /* 通过站点名称判断使用哪个连接 */
    if(site == "s4"){
        PORT = 7655;
        UNIX_SOCKET = "/home/mysql2/mysql.sock";
    }
    else{
        PORT = 7654;
        UNIX_SOCKET = "/home/mysql1/mysql.sock";
    } // 此处如果输入了没出现过的站点应当报警但我没写

    
    /* 构建cmd要执行的导入文件命令 */
    /* 举例：mysql -u root -S /home/mysql2/mysql.sock -prootroot -Dtest < /mnt/d/ddbms200tmp/tmp_book_1.sql */
    string command_save = "mysql -u ";
    command_save.append(USERNAME);
    command_save.append(" -S ");
    command_save.append(UNIX_SOCKET);
    command_save.append(" -p");
    command_save.append(PASSWORD);
    command_save.append(" -D");
    command_save.append(DATABASE);
    command_save.append(" ");
    command_save.append(" < ");
    command_save.append(TMPPATH);
    command_save.append(tmp_data);
    command_save.append(".sql");
    /* 执行 */
    const char* q = command_save.data();
    system(q);  //此处应当判断一下成功没有，但是我没写
    return "OK";

}

string Local_Select(string sql, string res_name, string site) 
{
    MYSQL my_connection; /*这是一个数据库连接*/
    int res; /*执行sql語句后的返回标志*/

    int PORT;
    const char* UNIX_SOCKET;

    /* 通过站点名称判断使用哪个连接 */
    if(site == "s4"){
        PORT = 7655;
        UNIX_SOCKET = "/home/mysql2/mysql.sock";
    }
    else{
        PORT = 7654;
        UNIX_SOCKET = "/home/mysql1/mysql.sock";
    } // 此处如果输入了没出现过的站点应当报警但我没写
 
    /*初始化mysql连接my_connection*/
    mysql_init(&my_connection);

    /*这里就是用了mysql.h里的一个函数，用我们之前定义的那些宏建立mysql连接，并
     *     返回一个值，返回不为空证明连接是成功的*/
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET, 0)) 
    {
        /*连接成功*/
        printf("connect success！\n");
        /*这句话是设置查询编码为utf8，这样支持中文*/
        mysql_query(&my_connection, "set names utf8");
    
        /*下面这句话就是用mysql_query函数来执行我们刚刚传入的sql語句，
         *         这会返回一个int值，如果为0，证明語句执行成功*/
        /* 构建新表存储查询结果的语句 */
        string new_sql = "create table ";
        new_sql.append(res_name);
        new_sql.append(" ");
        new_sql.append(sql);
        const char* p = new_sql.data();
        /* 执行 */
        res = mysql_query(&my_connection, p);
    
        if (res) 
        {   /*现在就代表执行失败了*/
            printf("Error： mysql_query !\n");
            return "FAIL";
        }
        else 
        { 
            /*现在就代表执行成功了*/
            /* 构建cmd要执行的导出文件命令 */
            /* 举例：mysqldump -u root -S /home/mysql1/mysql.sock -prootroot test book > book.sql */
            string command_save = "mysqldump -u ";
            command_save.append(USERNAME);
            command_save.append(" -S ");
            command_save.append(UNIX_SOCKET);
            command_save.append(" -p");
            command_save.append(PASSWORD);
            command_save.append(" ");
            command_save.append(DATABASE);
            command_save.append(" ");
            command_save.append(res_name);
            command_save.append(" > ");
            command_save.append(TMPPATH);
            command_save.append(res_name);
            command_save.append(".sql");
            /* 执行 */
            const char* q = command_save.data();
            system(q);  //此处应当判断一下成功没有，但是我没写
            /* 构建语句删除原来的表 */
            string new_sql = "drop table ";
            new_sql.append(res_name);
            const char* p = new_sql.data();
            /* 执行 */
            res = mysql_query(&my_connection, p);
        
            if (res) 
            {   /*现在就代表执行失败了*/
                printf("Error： mysql release error!\n");
            }
            else 
            { 
                /*现在就代表执行成功了*/
            }
            mysql_close(&my_connection);
            return res_name;
        }             
    }
    else{
        return "FAIL";
    }  
}

void my_mysql_res_print(string my_res_name)
{
    /* 将sql文件读入MySQL */
    MYSQL conn;
    int res;
    int res_load;
    MYSQL_RES *res_ptr; /*指向查询结果的指针*/
    MYSQL_FIELD *field; /*字段结构指针*/
    MYSQL_ROW result_row; /*按行返回的查询信息*/

    int row, column; /*查询返回的行数和列数*/
    int i, j; /*只是控制循环的两个变量*/

    int PORT;
    const char* UNIX_SOCKET;

    /* 现在直接默认使用第一个连接 */
    PORT = 7654;
    UNIX_SOCKET = "/home/mysql1/mysql.sock";

    /* 构建cmd要执行的导入文件命令 */
    /* 举例：mysql -u root -S /home/mysql2/mysql.sock -prootroot -Dtest < /mnt/d/ddbms200tmp/tmp_book_1.sql */
    string command_save = "mysql -u ";
    command_save.append(USERNAME);
    command_save.append(" -S ");
    command_save.append(UNIX_SOCKET);
    command_save.append(" -p");
    command_save.append(PASSWORD);
    command_save.append(" -D");
    command_save.append(DATABASE);
    command_save.append(" ");
    command_save.append(" < ");
    command_save.append(TMPPATH);
    command_save.append(my_res_name);
    command_save.append(".sql");
    /* 执行 */
    const char* q = command_save.data();
    system(q);  //此处应当判断一下成功没有，但是我没写

    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        /* 构建取表语句 */
        string sql_print = "select * from ";
        sql_print.append(my_res_name);
        /* 执行取表语句 */
        const char* p = sql_print.data();
        res_load=mysql_query(&conn,p);
        if(res_load){
            /* 取数据表失败 */
            printf("DATA READ FAILED\n");
        }
        else /* 取数据成功 */
        {
            /*将查询的結果给res_ptr*/
            res_ptr = mysql_store_result(&conn);
            /*如果结果不为空，就把结果print*/
            if (res_ptr) 
            {
                /*取得結果的行列数*/
                column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr);
                printf("查询到 %d 行 \n", row);

                /*输出結果的字段名*/
                for (i = 0; field = mysql_fetch_field(res_ptr); i++)
                {
                    printf("%s\t", field->name);
                }
                    
                printf("\n");

                /*按行输出結果*/
                for (i = 1; i <= row; i++)
                {
                    result_row = mysql_fetch_row(res_ptr);
                    for (j = 0; j < column; j++)
                    {
                        printf("%s\t", result_row[j]);
                    }
                    printf("\n"); 
            
                }
            }
            else
            {
                printf("Empty!");
            }
            /* 构建语句删除原来的表 */
            string new_sql = "drop table ";
            new_sql.append(my_res_name);
            const char* p = new_sql.data();
            /* 执行 */
            res = mysql_query(&conn, p);
        
            if (res) 
            {   /*现在就代表执行失败了*/
                printf("Error： mysql release error!\n");
            }
            else 
            { 
                /*现在就代表执行成功了*/
            }
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
        }
    }
            
}

void my_mysql_res_output(string my_res_name, string filename){
    /* 将sql文件读入MySQL */
    MYSQL conn;
    int res;
    int res_load;
    MYSQL_RES *res_ptr; /*指向查询结果的指针*/
    MYSQL_FIELD *field; /*字段结构指针*/
    MYSQL_ROW result_row; /*按行返回的查询信息*/

    int row, column; /*查询返回的行数和列数*/
    int i, j; /*只是控制循环的两个变量*/

    int PORT;
    const char* UNIX_SOCKET;

    /* 现在直接默认使用第一个连接 */
    PORT = 7654;
    UNIX_SOCKET = "/home/mysql1/mysql.sock";

    /* 构建cmd要执行的导入文件命令 */
    /* 举例：mysql -u root -S /home/mysql2/mysql.sock -prootroot -Dtest < /mnt/d/ddbms200tmp/tmp_book_1.sql */
    string command_save = "mysql -u ";
    command_save.append(USERNAME);
    command_save.append(" -S ");
    command_save.append(UNIX_SOCKET);
    command_save.append(" -p");
    command_save.append(PASSWORD);
    command_save.append(" -D");
    command_save.append(DATABASE);
    command_save.append(" ");
    command_save.append(" < ");
    command_save.append(TMPPATH);
    command_save.append(my_res_name);
    command_save.append(".sql");
    /* 执行 */
    const char* q = command_save.data();
    system(q);  //此处应当判断一下成功没有，但是我没写

    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        /* 构建取表语句 */
        string sql_print = "select * from ";
        sql_print.append(my_res_name);
        /* 执行取表语句 */
        const char* p = sql_print.data();
        res_load=mysql_query(&conn,p);
        if(res_load){
            /* 取数据表失败 */
            printf("DATA READ FAILED\n");
        }
        else /* 取数据成功 */
        {
            /*将查询的結果给res_ptr*/
            res_ptr = mysql_store_result(&conn);
            /*如果结果不为空，就把结果print*/
            if (res_ptr) 
            {
                /* 将输出重定向到文件filename */
                freopen(filename.data(), "w", stdout);
                /*取得結果的行列数*/
                column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr);
                // printf("查询到 %d 行 \n", row);

                /*输出結果的字段名*/
                for (i = 0; field = mysql_fetch_field(res_ptr); i++)
                {
                    printf("%s\t", field->name);
                }
                    
                printf("\n");

                /*按行输出結果*/
                for (i = 1; i <= row; i++)
                {
                    result_row = mysql_fetch_row(res_ptr);
                    for (j = 0; j < column; j++)
                    {
                        printf("%s\t", result_row[j]);
                    }
                    printf("\n"); 
            
                }
                /* 将输出重定向到控制台 */
                freopen("/dev/tty", "w", stdout);
            }
            else
            {
                printf("Empty!");
            }
            /* 构建语句删除原来的表 */
            string new_sql = "drop table ";
            new_sql.append(my_res_name);
            const char* p = new_sql.data();
            /* 执行 */
            res = mysql_query(&conn, p);
        
            if (res) 
            {   /*现在就代表执行失败了*/
                printf("Error： mysql release error!\n");
            }
            else 
            { 
                /*现在就代表执行成功了*/
            }
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
        }
    }
            

}

int my_mysql_res_get_rows(string my_res_name){
    /* 将sql文件读入MySQL */
    MYSQL conn;
    int res;
    int res_load;
    MYSQL_RES *res_ptr; /*指向查询结果的指针*/
    // MYSQL_FIELD *field; /*字段结构指针*/
    // MYSQL_ROW result_row; /*按行返回的查询信息*/

    int row, column; /*查询返回的行数和列数*/
    // int i, j; /*只是控制循环的两个变量*/

    int PORT;
    const char* UNIX_SOCKET;

    /* 现在直接默认使用第一个连接 */
    PORT = 7654;
    UNIX_SOCKET = "/home/mysql1/mysql.sock";

    /* 构建cmd要执行的导入文件命令 */
    /* 举例：mysql -u root -S /home/mysql2/mysql.sock -prootroot -Dtest < /mnt/d/ddbms200tmp/tmp_book_1.sql */
    string command_save = "mysql -u ";
    command_save.append(USERNAME);
    command_save.append(" -S ");
    command_save.append(UNIX_SOCKET);
    command_save.append(" -p");
    command_save.append(PASSWORD);
    command_save.append(" -D");
    command_save.append(DATABASE);
    command_save.append(" ");
    command_save.append(" < ");
    command_save.append(TMPPATH);
    command_save.append(my_res_name);
    command_save.append(".sql");
    /* 执行 */
    const char* q = command_save.data();
    system(q);  //此处应当判断一下成功没有，但是我没写

    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        /* 构建取表语句 */
        string sql_print = "select * from ";
        sql_print.append(my_res_name);
        /* 执行取表语句 */
        const char* p = sql_print.data();
        res_load=mysql_query(&conn,p);
        if(res_load){
            /* 取数据表失败 */
            printf("DATA READ FAILED\n");
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
            return 0;
        }
        else /* 取数据成功 */
        {
            /*将查询的結果给res_ptr*/
            res_ptr = mysql_store_result(&conn);
            /*如果结果不为空，就把结果print*/
            if (res_ptr) 
            {
                /*取得結果的行列数*/
                column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr);
            }
            else
            {
                row = 0;
                printf("Empty!");
            }
            /* 构建语句删除原来的表 */
            string new_sql = "drop table ";
            new_sql.append(my_res_name);
            const char* p = new_sql.data();
            /* 执行 */
            res = mysql_query(&conn, p);
        
            if (res) 
            {   /*现在就代表执行失败了*/
                printf("Error： mysql release error!\n");
            }
            else 
            { 
                /*现在就代表执行成功了*/
            }
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
            return row;
        }
    }
    else{
        return 0;
    }            
}

/* 获得SQL文件里第一列数据(此处其实特意为id而设)，要在load之前（之后再也不需要数据库里有这个数据）  */
vector<int> my_mysql_res_get_column1(string my_res_name){
    vector<int> data_output;
    /* 将sql文件读入MySQL */
    MYSQL conn;
    int res;
    int res_load;
    MYSQL_RES *res_ptr; /*指向查询结果的指针*/
    MYSQL_FIELD *field; /*字段结构指针*/
    MYSQL_ROW result_row; /*按行返回的查询信息*/

    int row, column; /*查询返回的行数和列数*/
    int i, j; /*只是控制循环的两个变量*/

    int PORT;
    const char* UNIX_SOCKET;

    /* 现在直接默认使用第一个连接 */
    PORT = 7654;
    UNIX_SOCKET = "/home/mysql1/mysql.sock";

    /* 构建cmd要执行的导入文件命令 */
    /* 举例：mysql -u root -S /home/mysql2/mysql.sock -prootroot -Dtest < /mnt/d/ddbms200tmp/tmp_book_1.sql */
    string command_save = "mysql -u ";
    command_save.append(USERNAME);
    command_save.append(" -S ");
    command_save.append(UNIX_SOCKET);
    command_save.append(" -p");
    command_save.append(PASSWORD);
    command_save.append(" -D");
    command_save.append(DATABASE);
    command_save.append(" ");
    command_save.append(" < ");
    command_save.append(TMPPATH);
    command_save.append(my_res_name);
    command_save.append(".sql");
    /* 执行 */
    const char* q = command_save.data();
    system(q);  //此处应当判断一下成功没有，但是我没写

    mysql_init(&conn);
    if(mysql_real_connect(&conn, HOST, USERNAME, PASSWORD, DATABASE, PORT, UNIX_SOCKET,0))
    {
        printf("connect success!\n");
        /* 构建取表语句 */
        string sql_print = "select * from " + my_res_name;
        /* 执行取表语句 */
        const char* p = sql_print.data();
        res_load=mysql_query(&conn,p);
        if(res_load){
            /* 取数据表失败 */
            printf("DATA READ FAILED\n");
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
            return data_output;
        }
        else /* 取数据成功 */
        {
            /*将查询的結果给res_ptr*/
            res_ptr = mysql_store_result(&conn);
            /*如果结果不为空，就把结果print*/
            if (res_ptr) 
            {
                /*取得結果的行列数*/
                // column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr);
                                /*按行输出結果*/
                for (i = 1; i <= row; i++)
                {
                    result_row = mysql_fetch_row(res_ptr);
                    const char* tmp0 = result_row[0];
                    int tmp = atoi(tmp0);
                    data_output.push_back(tmp);            
                }
            }
            else
            {
                printf("Empty!");
            }
            /* 构建语句删除原来的表 */
            string new_sql = "drop table " + my_res_name;
            const char* p = new_sql.data();
            /* 执行 */
            res = mysql_query(&conn, p);
        
            if (res) 
            {   /*现在就代表执行失败了*/
                printf("Error： mysql release error!\n");
            }
            else 
            { 
                /*现在就代表执行成功了*/
            }
            /* 关闭连接 */
            mysql_close(&conn);
            mysql_free_result(res_ptr);
            return data_output;
        }
    }
    else{
        return data_output;
    }            
}

/* res_ptr = mysql_store_result(&my_connection)用于分配内存并把指针返回
   mysql_free_result(res_ptr)则用于释放被分配的内存
   一般来说子站点返回结果集之后就应当释放，但不知道RPC如何反应，
   我暂且先写在executor的transfer后面吧，加上注释，一开始的测试通过以后再尝试取消注释能不能继续跑通*/
/* mysql_library_end()用于彻底释放，按理来说mysql_free_result如果有，这个也应当有，
   但是它并不指定是哪一个连接，我觉得多线程的时候有些危险，所以不打算放上去*/
