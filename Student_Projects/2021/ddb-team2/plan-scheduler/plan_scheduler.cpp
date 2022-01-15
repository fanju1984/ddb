/* plan_scheduler.cpp

    Query-plan scheduling.

    Created by khaiwang

    2021/11/16
*/

#include "plan_scheduler.h"
// #include "network.h"
#include "meta.h"

DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
                                 "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
                              "(waiting for client to close connection before server stops)");
DEFINE_int32(max_concurrency, 0, "Limit of request processing in parallel");

// conf for client
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 3000000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_string(protocol, "baidu_std", "Client-side protocol");
DEFINE_bool(server_init, false, "if cleanup and init the server");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
const std::string ETCD_PUT_URL = "http://127.0.0.1:2379/v3/kv/put";
const std::string ETCD_GET_URL = "http://127.0.0.1:2379/v3/kv/range";
const std::string ETCD_DELETE_URL = "http://127.0.0.1:2379/v3/kv/deleterange";
std::string DB_NAME;

/* clean-up the environment and init
*/
int init_server() {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::ResultSet* rs;

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "SecretPassword@123");
    stat = conn->createStatement();

    try {
        stat->execute("drop database if exists `" + DB_NAME + "`;");
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return -1;
    }

    try {
        stat->execute("create database `" + DB_NAME + "`;");
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return -1;
    }
    return 0;
}

/* execute sql rpc
*/
int execute_query_sql(const std::string &sql, ExecuteQuerySQLResponse* response) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::ResultSet* rs;

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "SecretPassword@123");
    stat = conn->createStatement();

    try {
        stat->execute("create database if not exists `" + DB_NAME + "`;");
        stat->execute("use `" + DB_NAME + "`;");
        // std::auto_ptr<sql::ResultSet> rs(stat->executeQuery(sql));
        rs = stat->executeQuery(sql);
        LOG(INFO) << "query result row count: " << rs->rowsCount();
        sql::ResultSetMetaData *rsm = rs->getMetaData();
        int count = rsm->getColumnCount();
        if (count > 0) {
//            std::map<std::string, std::string> columns;
            std::vector<std::string> field_names;
            std::vector<std::string> field_types;
            std::string field_meta;

            // field_meta = "";
            field_meta = rsm->getColumnName(1) + " " + rsm->getColumnTypeName(1);
            field_names.push_back(rsm->getColumnName(1));
            field_types.push_back(rsm->getColumnTypeName(1));
            for (int i = 2; i <= count; i++) {
                field_meta += (", `" + rsm->getColumnName(i) + "` " + rsm->getColumnTypeName(i));
                field_names.push_back(rsm->getColumnName(i));
                field_types.push_back(rsm->getColumnTypeName(i));
            }
            
            response->set_table_fields(field_meta);
            while (rs->next()) {
                std::string row;
                for (int i = 0, j = 0; i < field_names.size() && j < field_types.size();  ++i, ++j) {
                    row += rs->getString(field_names[i]) + ",";
                }
                response->add_table_values(row.substr(0, row.length() - 1));
            }
        }
        rs->close();
        stat->close();
        conn->close();
        // MysqlPool.ReleaseConnection(conn);
        return count;
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return 0;
    }
}

int execute_query_sql(const std::string &sql, const std::string &table_name, NodeResponse* response, const Node_type node_type) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::ResultSet* rs;
    LOG(INFO) << "execute query sql: ";
    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "SecretPassword@123");
    stat = conn->createStatement();

    try {
        
        stat->execute("create database if not exists `" + DB_NAME + "`;");
        stat->execute("use `" + DB_NAME + "`;");
        // std::auto_ptr<sql::ResultSet> rs(stat->executeQuery(sql));
        LOG(INFO) << "query sql: "  << std::endl << sql;

        rs = stat->executeQuery(sql);
        LOG(INFO) << "query result row count: " << rs->rowsCount();
        sql::ResultSetMetaData *rsm = rs->getMetaData();
        int count = rsm->getColumnCount();
       
        if (count > 0) {
//            std::map<std::string, std::string> columns;
            std::vector<std::string> field_names;
            std::vector<std::string> field_types;
            std::string field_meta;
            if (node_type == NODE_LEAF) {
                field_meta = "`" + rsm->getColumnName(1) + "` " + rsm->getColumnTypeName(1) + "(25) default null";
                field_names.push_back(rsm->getColumnName(1));
                field_types.push_back(rsm->getColumnTypeName(1));
                for (int i = 2; i <= count; i++) {
                    field_meta += (", `" + rsm->getColumnName(i) + "` " + rsm->getColumnTypeName(i) + "(25) default null");
                    field_names.push_back(rsm->getColumnName(i));
                    field_types.push_back(rsm->getColumnTypeName(i));
                }
            }
            else {    
                field_meta = "`" + rsm->getColumnName(1) + "` " + rsm->getColumnTypeName(1) + "(25) default null";
                field_names.push_back(rsm->getColumnName(1));
                field_types.push_back(rsm->getColumnTypeName(1));
                for (int i = 2; i <= count; i++) {
                    field_meta += ", `" + rsm->getColumnName(i) + "` " + rsm->getColumnTypeName(i) + "(25) default null";
                    field_names.push_back(rsm->getColumnName(i));
                    field_types.push_back(rsm->getColumnTypeName(i));
                }
            }
            response->set_table_fields(field_meta);
            while (rs->next()) {
                std::string row;
                for (int i = 0, j = 0; i < field_names.size() && j < field_types.size();  ++i, ++j) {
                    if(field_types[j] == "VARCHAR") {
                        row += "\"" + rs->getString(field_names[i]) + "\",";
                    }
                    else if(field_types[j] == "INT") {
                        row += rs->getString(field_names[i]) + ",";
                    }
                }
                response->add_table_values(row.substr(0, row.length() - 1));
            }
        }
        rs->close();
        stat->close();
        conn->close();
        // MysqlPool.ReleaseConnection(conn);
        return count;
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return 0;
    }
}

bool execute_non_query_sql(const std::string & sql) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "SecretPassword@123");
    stat = conn->createStatement();
    // LOG(INFO) << sql;

    try {
        stat->execute("create database if not exists `" + DB_NAME + "`;");
        stat->execute("use `" + DB_NAME + "`;");
        // std::auto_ptr<sql::ResultSet> rs(stat->executeQuery(sql));
        bool rs = stat->execute(sql);

        
        stat->close();
        conn->close();
        // MysqlPool.ReleaseConnection(conn);
        return rs;
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return -1 ;
    }
}

void trim(std::string &s) 
{
    if (s.empty()) 
    {
        return ;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
}

class MyCallMapper : public brpc::CallMapper {
public:
//    MyCallMapper(std::string mTempTableName) : m_temp_table_name(std::move(mTempTableName)) {}
    MyCallMapper(std::string mTempTableName, bool isQuery) : m_temp_table_name(std::move(mTempTableName)),
                                                                    m_is_query(isQuery) {}

public:
    brpc::SubCall
    Map(int channel_index, const google::protobuf::MethodDescriptor *method, const google::protobuf::Message *request,
        google::protobuf::Message *response) override {
        LOG(INFO) << typeid(*request).name();
        std::string request_name = typeid(*request).name();
        if (request_name.find("DeleteTempTableRequest") != std::string::npos){
            LOG(INFO) << "delete temp table :" << m_temp_table_name;
            DeleteTempTableRequest* delete_table_request = brpc::Clone<DeleteTempTableRequest>(request);
            delete_table_request->set_temp_table_name(m_temp_table_name);
            return brpc::SubCall(method, delete_table_request, response->New(), brpc::DELETE_REQUEST | brpc::DELETE_RESPONSE);
        }
        else if (request_name.find("NodeRequest") != std::string::npos){
            NodeRequest* node_request = brpc::Clone<NodeRequest>(request);
            node_request->set_temp_table_name(m_temp_table_name);
            return brpc::SubCall(method, node_request, response->New(), brpc::DELETE_REQUEST | brpc::DELETE_RESPONSE);
        }

    }

private:
    std::string m_temp_table_name;
    bool m_is_query;
};

class MyResponseMerger : public brpc::ResponseMerger {
public:
    MyResponseMerger(bool mIsUnion, const std::string &mTempTableName, const std::string &mIndexAttr) : m_is_union(
            mIsUnion), m_temp_table_name(mTempTableName), m_index_attr(mIndexAttr) {}

    MyResponseMerger(bool mIsUnion, const std::string &mTempTableName) : m_is_union(mIsUnion),
                                                                         m_temp_table_name(mTempTableName) {}

public:
    Result Merge(google::protobuf::Message *response, const google::protobuf::Message *sub_response) override {
        std::string response_name = typeid(*response).name();
        if (response_name.find("DeleteTempTableResponse") != std::string::npos){

        }
        else if (response_name.find("NodeResponse") != std::string::npos){
            // 1. union -- merge before create temp table
            LOG(INFO) << "deal with sub return : ";
            NodeResponse* nodeResponse = (NodeResponse *) sub_response;
            LOG(INFO) << "response size : " << nodeResponse->table_values_size();
            if (m_is_union) {
                response->MergeFrom(*sub_response);
            }
                // 2. join or leaf-node -- create temp table before join
            else {
                LOG(INFO) << "merge temp table result: " << m_temp_table_name;
                std::string drop_table_sql = "drop table if exists `" + m_temp_table_name + "`;";
                execute_non_query_sql(drop_table_sql);
                std::string create_table_sql = "create table `" + m_temp_table_name + "` ( " + nodeResponse->table_fields();
                if (m_index_attr != ""){
                    create_table_sql = create_table_sql + ", " + "index (" + m_index_attr + ")";
                } 
                
                create_table_sql = create_table_sql + " ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
                LOG(INFO) << "create temp table " << std::endl << create_table_sql;
                execute_non_query_sql(create_table_sql);

                if (nodeResponse->table_values_size() == 0) {
                    LOG(ERROR) << "no response!";
                } else {
                    std::string insert_table_sql =
                            "insert into `" + m_temp_table_name + "` values (" + nodeResponse->table_values(0) + ")";
                    for (int i = 1; i < nodeResponse->table_values_size(); ++i) {
                        insert_table_sql += ", (" + nodeResponse->table_values(i) + ")";
                    }
                    insert_table_sql += ";";
                    execute_non_query_sql(insert_table_sql);
                }
            }
        }


        return MERGED;
    }

private:
    bool m_is_union;
    std::string m_temp_table_name;
    std::string m_index_attr;
};

nlohmann::json read_from_etcd(const std::string &key) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = "http";
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(ETCD_GET_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }
    nlohmann::json kv;

    kv["key"]  = base64_encode(key);
    
    brpc::Controller cntl;
    cntl.http_request().uri() = ETCD_GET_URL;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(kv.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);

    if(cntl.Failed()){
        LOG(ERROR) << cntl.ErrorText() << std::endl;
        return NULL;
    }

    kv.clear();
    std::string temp = cntl.response_attachment().to_string();
    // cntl.Reset();
    kv = nlohmann::json::parse(temp);
    if (kv["count"].type_name() == "null"){
        return NULL;
    }
    temp = kv["count"];
    if (std::stoi(temp) == 0){
        return NULL;
    }
    return kv;
}

std::string read_kv_from_etcd(const std::string &key) {
    nlohmann::json kv = read_from_etcd(key);
    if(kv != NULL) {
        std::string temp = kv["count"];
        int count = std::stoi(temp);
        if(count > 0) {
            std::string result = kv["kvs"][0]["value"];
            LOG(INFO) << "read " << key << " from etcd, receives " << result;
            return base64_decode(result);
        }
    }
    return "";
}



void SQLServiceImpl::RequestNode(::google::protobuf::RpcController *controller, const ::NodeRequest *request,
                      ::NodeResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = dynamic_cast<brpc::Controller *>(controller);
    const std::string temp_table_name = request->temp_table_name();
    nlohmann::json temp_table_json = nlohmann::json::parse(read_kv_from_etcd(temp_table_name));
    Node temp_table(temp_table_json);
    LOG(INFO) << "get node: " << temp_table_json.dump();
    if(temp_table.get_node_type() == NODE_LEAF) {
        std::string temp_table_name = temp_table.get_children().begin()->first;
        if(temp_table.get_project_predicate().empty()) {
            temp_table.set_project_predicate("*");
        }
        std::string query = "select " + temp_table.get_project_predicate() + " from `" + temp_table_name + "`";
        if(!temp_table.get_select_predicate().empty()) {
            query = query + " where " + temp_table.get_select_predicate();
        }
        query += ";";
        //std::string table_name = temp_table_name.substr(temp_table_name.find('_') + 1);
        LOG(INFO) << "leaf node query: " << query;
        int row_count = execute_query_sql(query, temp_table_name, response, temp_table.get_node_type());
        // if (row_count <= 0) {
        //     cntl->SetFailed("empty result for sql: " + query);
        // } 
        return;
    }
    else if(temp_table.get_node_type() == NODE_NOT_LEAF) {
        brpc::ParallelChannel channel;
        brpc::ParallelChannelOptions pchan_options;
        pchan_options.timeout_ms = FLAGS_timeout_ms;
        if (channel.Init(&pchan_options) != 0) {
            LOG(ERROR) << "Fail to init ParallelChannel!";
            cntl->SetFailed("Fail to init ParallelChannel for child requests!");
            return;
        }
        brpc::ChannelOptions sub_options;
        sub_options.protocol = FLAGS_protocol;
        sub_options.connection_type = FLAGS_connection_type;
        sub_options.max_retry = FLAGS_max_retry;

        std::string first_child_temp_table_name = temp_table.get_children().begin()->first;
        if(temp_table.get_children().size() == 1) {
            temp_table.set_is_union(true);
        }
        std::map<std::string, std::string> join_attrs;
        if (!temp_table.get_is_union()){
            std::string join_predicate = temp_table.get_join_predicate();
            LOG(INFO) << "join_predicate: " << join_predicate;
            std::string temp = join_predicate.substr(0, join_predicate.find('='));
            int index = temp.find('.');
            std::string key = temp.substr(0, index);
            std::string value = temp.substr(index + 1);
            join_attrs[key] = value;
            temp = join_predicate.substr(join_predicate.find_last_of('=') + 1);
            index = temp.find('.');
            key = temp.substr(0, index);
            value = temp.substr(index + 1);
            join_attrs[key] = value;
        }

        std::vector<std::string> child_temp_table_names;
        std::map<std::string, std::string> children = temp_table.get_children();
        for (auto iter = children.begin(); iter != children.end(); iter++) {
            LOG(INFO) << temp_table_name << " ---> " << iter->first;
            child_temp_table_names.push_back(iter->first);
            MyCallMapper *mapper = new MyCallMapper(iter->first, true);
            MyResponseMerger *merger;
            if (temp_table.get_is_union()){
                merger = new MyResponseMerger(temp_table.get_is_union(), iter->first);
            } else{
                merger = new MyResponseMerger(temp_table.get_is_union(), iter->first, join_attrs[iter->first]);
            }
            brpc::Channel *sub_channel = new brpc::Channel;
            std::string server_addr = iter->second;
            if (sub_channel->Init(server_addr.c_str(), &sub_options) != 0) {
                LOG(ERROR) << "Fail to initialize sub_channel for " << server_addr;
                cntl->SetFailed("Fail to initialize sub_channel for " + server_addr);
                return;
            }
            if (channel.AddChannel(sub_channel, brpc::OWNS_CHANNEL, mapper, merger) != 0) {
                LOG(ERROR) << "Fail to add sub_channel for " << server_addr;
                cntl->SetFailed("Fail to add sub_channel for " + server_addr);
                return;
            }
        }
        if (channel.channel_count() > 0) {
            
            sql_service_Stub stub(&channel);
            NodeRequest sub_request;
            NodeResponse sub_response;
            brpc::Controller sub_cntl;
            DeleteTempTableRequest delete_request;
            DeleteTempTableResponse delete_response;
            brpc::Controller delete_cntl;

            //LOG(INFO) << sub_request.temp_table_name();
            stub.RequestNode(&sub_cntl, &sub_request, &sub_response, NULL);
            bool failed = false;
            if (!sub_cntl.Failed()) {
                LOG(INFO) << "return from sub tree:" << temp_table_name;
                if (temp_table.get_is_union()) {
                    LOG(INFO) << temp_table_name << "'s combine operator is union, begin to create result temp table named first child : " << first_child_temp_table_name;
             
                    std::string drop_table_sql = "drop table if exists `" + first_child_temp_table_name + "`;";
                    execute_non_query_sql(drop_table_sql);
                    std::string create_table_sql =
                            "create table `" + first_child_temp_table_name + "` ( " + sub_response.table_fields() +
                            " ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
                    execute_non_query_sql(create_table_sql);
                    if (sub_response.table_values_size() == 0) {
                        LOG(ERROR) << "no response!";
                    } else {
                        std::string insert_table_sql =
                                "insert into `" + first_child_temp_table_name + "` values (" +
                                sub_response.table_values(0) + ")";
                        for (int i = 1; i < sub_response.table_values_size(); ++i) {
                            insert_table_sql += ", (" + sub_response.table_values(i) + ")";
                        }
                        insert_table_sql += ";";
                        execute_non_query_sql(insert_table_sql);
                    }

                    std::string project_predicate = temp_table.get_project_predicate();
                    if (project_predicate.empty()) {
                        project_predicate = "*";
                    }
                    // LOG(INFO) << "project of " << temp_table_name << ": " << tb.project_expr;
                    std::string sql = "select " + project_predicate + " from " + first_child_temp_table_name;
                    std::string select_predicate = temp_table.get_select_predicate();
                    if (!select_predicate.empty()) {
                        sql += " where " + select_predicate;
                    }
                    sql += ";";
                    // LOG(INFO) << "sql before execute of  " << temp_table_name << ": " << sql; 
                    int count = execute_query_sql(sql, first_child_temp_table_name, response, NODE_NOT_LEAF);
                    if (count <= 0) {
                        cntl->SetFailed("empty result for sql: " + sql);
                    }
                }
                else {

                    LOG(INFO) << temp_table_name << "'s combine operator is join, begin to execute join in mysql";

                    std::string project_predicate = temp_table.get_project_predicate();
                    if (project_predicate.empty()) {
                        project_predicate = "*";
                    }
                    std::string sql = "select " + project_predicate + " from ";
                    auto iter = temp_table.get_children().begin();
                    
                    LOG(INFO) << "left table: " << iter->first;
                    sql = sql + iter->first + " join ";
                    iter++;
                    LOG(INFO) << "right table: " << iter->first;
                    sql = sql + iter->first;

                    int index = temp_table.get_join_predicate().find('=');
                    std::string s1 = temp_table.get_join_predicate().substr(0, index);
                    std::string s2 = temp_table.get_join_predicate().substr(index + 1);
                    trim(s1);
                    trim(s2);
                    std::string attr1 = s1.substr(s1.find('.') + 1);
                    std::string attr2 = s2.substr(s2.find('.') + 1);
                    LOG(INFO) << "a1 " << attr1 << " a2 " << attr2;
                    if (attr1 == attr2)
                    {
                        sql += " using(" + attr1 + ")";
                    }
                    else{
                        sql += " on " + temp_table.get_join_predicate();
                    }

                    std::string join_predicate = temp_table.get_join_predicate();
                    if (join_predicate == "") {
                        // if there is no join predicate, use cross join
                        join_predicate = "1=1";
                    }
                                        
                    

                    // if (!temp_table.get_select_predicate().empty()) {
                    //     sql += " where " + temp_table.get_select_predicate();
                    // }
                    sql += ";";
                    LOG(INFO) << "join sql query: " << sql;
                    int count = execute_query_sql(sql, first_child_temp_table_name, response, NODE_NOT_LEAF);
                    if (count <= 0) {
                        cntl->SetFailed("empty result for sql: " + sql);
                    } 
                }
                LOG(INFO) << "Delete temp table " << temp_table_name;
                stub.DeleteTable(&delete_cntl, &delete_request, &delete_response, NULL);

            } else {
                LOG(ERROR) << "cascading rpc request failed!";
                LOG(ERROR) << sub_cntl.ErrorText();
                cntl->SetFailed("cascading rpc request failed!");
                return;
            }
        }
    }
}


void SQLServiceImpl::ImportData(::google::protobuf::RpcController *controller, const ::ImportDataRequest *request,
                   ::ImportDataResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = dynamic_cast<brpc::Controller *>(controller);
    std::string table_name = request->table_name();
    std::string table_fields = request->table_fields();
    std::string create_table = "create table if not exists `" + table_name + "` (" + table_fields + ")"
                                + "engine=InnoDB default charset=utf8;";

    execute_non_query_sql(create_table);
    if (request->table_values_size() == 0) {
        LOG(ERROR) << "no data!";
        response->set_result("zero rows to be inserted!");
    } else {
        std::string insert_table_sql =
                "insert into `" + table_name + "` values (" + request->table_values(0) + ")";
        for (int i = 1; i < request->table_values_size(); ++i) {
            insert_table_sql += ", (" + request->table_values(i) + ")";
        }
        insert_table_sql += ";";
        execute_non_query_sql(insert_table_sql);
        response->set_result("succeed");
    }
}

void SQLServiceImpl::DeleteTable(::google::protobuf::RpcController *controller, const ::DeleteTempTableRequest *request,
                     ::DeleteTempTableResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = dynamic_cast<brpc::Controller *>(controller);
    std::string temp_table_name = request->temp_table_name();
    std::string sql = "drop table if exists `" + temp_table_name + "`;";
    execute_non_query_sql(sql);
    response->set_result("succeed!");
}

void SQLServiceImpl::ExecuteNonQuerySQL(::google::protobuf::RpcController *controller, const ::ExecuteNonQuerySQLRequest *request,
                            ::ExecuteNonQuerySQLResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = dynamic_cast<brpc::Controller *>(controller);
    const std::string& sql = request->sql_query();
    execute_non_query_sql(sql);
}

void SQLServiceImpl::ExecuteQuerySQL(::google::protobuf::RpcController* controller,
                       const ::ExecuteQuerySQLRequest* request,
                       ::ExecuteQuerySQLResponse* response,
                       ::google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller *cntl = dynamic_cast<brpc::Controller *>(controller);
    const std::string& sql = request->sql();
    execute_query_sql(sql, response);
}

int main(int argc, char* argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    
    
    DB_NAME = "test_" + std::to_string(FLAGS_port);

    if (FLAGS_server_init && init_server() != 0) {
        LOG(ERROR) << "Fail to init environmnet";
    }

    // Generally you only need one Server.
    brpc::Server server;

    // Instance of your service.
    SQLServiceImpl ddb_service_impl;

    // Add the service into server. Notice the second parameter, because the
    // service is put on stack, we don't want server to delete it, otherwise
    // use brpc::SERVER_OWNS_SERVICE.
    if (server.AddService(&ddb_service_impl,
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    // Start the server.
    brpc::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    options.max_concurrency = FLAGS_max_concurrency;
    if (server.Start(FLAGS_port, &options) != 0) {
        LOG(ERROR) << "Fail to start DDB server";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}