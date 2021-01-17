#include "gflags/gflags.h"
#include "brpc/channel.h"
#include "brpc/parallel_channel.h"
#include "brpc/server.h"

#include "ddb.pb.h"
#include "json.hpp"
#include "base64.h"
#include "MysqlConnectionPool.hpp"


#include <iterator>
#include <utility>

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <regex>

#include <typeinfo>

// conf for server
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
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");


enum readtype {
    KEY = 0,
    PREFIX = 1
};


// node type
const std::string LEAF = "L";
const std::string NON_LEAF = "NL";

// combine op
const std::string UNION = "U";
const std::string JOIN = "J";

// seperator
const std::string OUTER_SEPERATOR = "|";
const std::string INNER_SEPERATOR = ":";

const std::string ETCD_GET_URL = "http://127.0.0.1:2379/v3/kv/range";
const std::string ETCD_PUT_URL = "http://127.0.0.1:2379/v3/kv/put";

const std::string TEMP_TABLE_PREFIX = "temp-table";

const std::string DB_NAME = "test";

const std::string QUERY_PROCESSING_STATISTICS_PREFIX = "/query_processing_statistics";

struct temp_table {
    int ret_code{};
    std::string type;
    std::string project_expr;
    std::string select_expr;
    bool is_union{};
    std::string join_expr;
    std::unordered_map<std::string, std::string> children;
};

bool execute_non_query_sql(const std::string &sql) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;

    // sql::Connection* conn = MysqlPool.GetConnection();
    // while (conn == NULL)
    // {
    //     LOG(ERROR) << "cannot get connection, wait 10ms";
    //     sleep(0.01);
    //     conn = MysqlPool.GetConnection();
    // }

    // std::auto_ptr<sql::Statement> stat(conn->createStatement());

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "123456");
    stat = conn->createStatement();
    if (sql.length() <= 400){
        LOG(INFO) << "execute sql: " << sql;
    } else{
        LOG(INFO) << "execute sql: " << sql.substr(0, 400) << " ....";
    }

    try {
        stat->execute("create database if not exists `" + DB_NAME + "`;");
        stat->execute("use `" + DB_NAME + "`;");
        bool result = stat->execute(sql);
        
        // MysqlPool.ReleaseConnection(conn);
        stat->close();
        conn->close();
        return result;
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return -1;
    }

}

int execute_query_sql(const std::string& sql, ExecuteQuerySQLResponse* response){
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::ResultSet* rs;
    // sql::Connection* conn = MysqlPool.GetConnection();
    // while (conn == NULL)
    // {
    //     LOG(ERROR) << "cannot get connection, wait 10ms";
    //     sleep(0.01);
    //     conn = MysqlPool.GetConnection();
    // }

    // std::auto_ptr<sql::Statement> stat(conn->createStatement());

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "123456");
    stat = conn->createStatement();
    if (sql.length() <= 400){
        LOG(INFO) << "execute sql: " << sql;
    } else{
        LOG(INFO) << "execute sql: " << sql.substr(0, 400) << " ....";
    }

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
            std::vector<std::string> column_names;
            std::vector<std::string> column_types;
            std::string attr_meta;

            attr_meta = rsm->getColumnName(1) + " " + rsm->getColumnTypeName(1);
            column_names.emplace_back(rsm->getColumnName(1));
            column_types.emplace_back(rsm->getColumnTypeName(1));
            for (int i = 2; i <= count; i++) {
                attr_meta += ("," + rsm->getColumnName(i) + " " + rsm->getColumnTypeName(i));
                column_names.emplace_back(rsm->getColumnName(i));
                column_types.emplace_back(rsm->getColumnTypeName(i));
            }
            
            response->set_attr_meta(attr_meta);
            while (rs->next()) {
                std::string row;
                for (int i = 0, j = 0; i < column_names.size() && j < column_types.size();  ++i, ++j) {
                    row += rs->getString(column_names[i]) + ",";
                }
                response->add_attr_values(row.substr(0, row.length() - 1));
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

int execute_query_sql(const std::string &sql, const std::string &table_name, TableResponse *response,
                      const std::string &temp_table_type) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *conn;
    sql::Statement *stat;
    sql::ResultSet* rs;

    // sql::Connection* conn = MysqlPool.GetConnection();
    // while (conn == NULL)
    // {
    //     LOG(ERROR) << "cannot get connection, wait 10ms";
    //     sleep(0.01);
    //     conn = MysqlPool.GetConnection();
    // }

    // std::auto_ptr<sql::Statement> stat(conn->createStatement());

    driver = sql::mysql::get_driver_instance();
    conn = driver->connect("127.0.0.1", "root", "123456");
    stat = conn->createStatement();
    if (sql.length() <= 400){
        LOG(INFO) << "execute sql: " << sql;
    } else{
        LOG(INFO) << "execute sql: " << sql.substr(0, 400) << " ....";
    }
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
            std::vector<std::string> column_names;
            std::vector<std::string> column_types;
            
            std::string attr_meta;
            // 1.leaf -- add table_name prefix
            if (temp_table_type == LEAF) {
                attr_meta = ("`" + table_name + "_" + rsm->getColumnName(1) + "` " +
                             rsm->getColumnTypeName(1) + "(20) default null");
                column_names.emplace_back(rsm->getColumnName(1));
                column_types.emplace_back(rsm->getColumnTypeName(1));
                for (int i = 2; i <= count; i++) {
                    attr_meta += (", `" + table_name + "_" + rsm->getColumnName(i) + "` " + rsm->getColumnTypeName(i) + "(20) default null");
                    column_names.emplace_back(rsm->getColumnName(i));
                    column_types.emplace_back(rsm->getColumnTypeName(i));
                }
            }
                // 2.non-leaf
            else {
                attr_meta = ("`" + rsm->getColumnName(1) + "` " +
                             rsm->getColumnTypeName(1)  + "(20) default null");
                column_names.emplace_back(rsm->getColumnName(1));
                column_types.emplace_back(rsm->getColumnTypeName(1));
                for (int i = 2; i <= count; i++) {
                    attr_meta += (", `" + rsm->getColumnName(i) + "` " + rsm->getColumnTypeName(i) + "(20) default null");
                    column_names.emplace_back(rsm->getColumnName(i));
                    column_types.emplace_back(rsm->getColumnTypeName(i));
                }
            }
            response->set_attr_meta(attr_meta);
            while (rs->next()) {
                std::string row;
                for (int i = 0, j = 0; i < column_names.size() && j < column_types.size();  ++i, ++j) {
                    if (column_types[j] == "VARCHAR"){
                        row += "'" + rs->getString(column_names[i]) + "',";
                    } else if (column_types[j] == "INT"){
                        row += rs->getString(column_names[i]) + ",";
                    }
                }
                response->add_attr_values(row.substr(0, row.length() - 1));
            }
        }
        // MysqlPool.ReleaseConnection(conn);
        rs->close();
        stat->close();
        conn->close();
        return count;
    } catch (sql::SQLException &exception) {
        LOG(ERROR) << exception.what();
        return 0;
    }

}

void write_query_processing_statistics_to_etcd(std::vector<std::string> temp_table_names, std::vector<long> latencies,
                                               std::vector<long> communication_costs) {
    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = "http";
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel when writing query processing statistics!";
        return;
    }

    brpc::Controller cntl;

    for (int i = 0; i < temp_table_names.size(); ++i) {
        // 1.latency
        std::string key = temp_table_names[i] + ".latency";
        std::string value = std::to_string(latencies[i]);
        //LOG(INFO) << "put <" << key << "," << value << ">";
        nlohmann::json j;
        j["key"] = base64_encode(key);
        j["value"] = base64_encode(value);
        cntl.http_request().uri() = ETCD_PUT_URL;
        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if (cntl.Failed()) {
            LOG(ERROR) << cntl.ErrorText() << std::endl;
            //return -1;
        }
        cntl.Reset();

        // 2.communication cost
        key = temp_table_names[i] + ".communication-cost";
        value = std::to_string(communication_costs[i]);
        //LOG(INFO) << "put <" << key << "," << value << ">";
        j["key"] = base64_encode(key);
        j["value"] = base64_encode(value);
        cntl.http_request().uri() = ETCD_PUT_URL;
        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if (cntl.Failed()) {
            LOG(ERROR) << cntl.ErrorText() << std::endl;
            //return -1;
        }
        cntl.Reset();
    }
}

std::string get_range_end_for_prefix(std::string s) {
    for (int i = 0; i < s.length(); i++) {
        if (s[i] < 0xff) {
            s[i]++;
            return s.substr(0, i + 1);
        }
    }
    return "";
}

nlohmann::json read_from_etcd(brpc::Channel *channel, brpc::Controller *cntl, const std::string &key, readtype type) {


    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl->http_request().uri() = ETCD_GET_URL;

    nlohmann::json j;
    j["key"] = base64_encode(key);
    if (type == PREFIX) {
        j["range_end"] = base64_encode(get_range_end_for_prefix(key));
    }
    cntl->request_attachment().append(j.dump());
    channel->CallMethod(NULL, cntl, NULL, NULL, NULL);
    if (cntl->Failed()) {
        LOG(ERROR) << cntl->ErrorText();
        return NULL;
    }
    j.clear();
    std::string temp = cntl->response_attachment().to_string();
    cntl->Reset();
//    LOG(INFO) << "read " << key << " from etcd, receives " << temp;
    j = nlohmann::json::parse(temp);
    if (j["count"].type_name() == "null"){
        return NULL;
    }
    temp = j["count"];
    if (std::stoi(temp) == 0){
        return NULL;
    }
    return j;
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

std::vector<std::string> s_split(const std::string &in, const std::string &delim) {
   std::regex re{delim};
   return std::vector<std::string>{
           std::sregex_token_iterator(in.begin(), in.end(), re, -1),
           std::sregex_token_iterator()
   };
}

void split_string(const std::string &s, std::vector<std::string> &v, const std::string &c) {
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

temp_table read_temp_table_meta(const std::string &temp_table_name) {
    temp_table tb;
    tb.ret_code = -1;
    tb.is_union = false;

    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = "http";
    options.timeout_ms = 20000;
    options.max_retry = 3;
    if (channel.Init(ETCD_GET_URL.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return tb;
    }
    brpc::Controller cntl;

    std::string prefix = temp_table_name;
    std::string temp;
    // 1.type
    std::string key = prefix + ".type";
    nlohmann::json j = read_from_etcd(&channel, &cntl, key, KEY);
    if (j == NULL) {
        return tb;
    }
//    temp = j["count"];
//    int count = std::stoi(temp);
//    if (count == 0) {
//        return tb;
//    }
    temp = j["kvs"][0]["value"];
    tb.type = base64_decode(temp);

    // 2.project
    key = prefix + ".project";
    j = read_from_etcd(&channel, &cntl, key, KEY);
    if (j != NULL) {
//        temp = j["count"];
//        int count = std::stoi(temp);
//        if (count > 0) {
            temp = j["kvs"][0]["value"];
            tb.project_expr = base64_decode(temp);
//        }
    }

    // 3.select
    key = prefix + ".select";
    j = read_from_etcd(&channel, &cntl, key, KEY);
    if (j != NULL) {
//        temp = j["count"];
//        count = std::stoi(temp);
//        if (count > 0) {
            temp = j["kvs"][0]["value"];
            tb.select_expr = base64_decode(temp);
//        }
    }

    // 4.combine
    key = prefix + ".combine";
    j = read_from_etcd(&channel, &cntl, key, KEY);
    if (j != NULL) {
//        temp = j["count"];
//        count = std::stoi(temp);
//        if (count > 0) {
            temp = j["kvs"][0]["value"];
            temp = base64_decode(temp);
            if (temp == UNION) {
                tb.is_union = true;
            } else {
                tb.join_expr = temp;
            }
//        }
    }

    // 5.children
    key = prefix + ".children";
    j = read_from_etcd(&channel, &cntl, key, KEY);
    if (j == NULL) {
        return tb;
    }
//    temp = j["count"];
//    count = std::stoi(temp);
//    if (count == 0) {
//        return tb;
//    }
    temp = j["kvs"][0]["value"];
    std::vector<std::string> cs;
    split_string(base64_decode(temp), cs, OUTER_SEPERATOR);
    for (const auto& s : cs) {
        int index = s.find(INNER_SEPERATOR, 0);
        std::string temp_table_name1 = s.substr(0, index);
        std::string ip = s.substr(index + 1, s.length() - index);
        tb.children[temp_table_name1] = ip;
    }

    tb.ret_code = 0;
    return tb;
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
//        LOG(INFO) << typeid(*request).name();
        std::string request_name = typeid(*request).name();
        if (request_name.find("DeleteTempTableRequest") != std::string::npos){
            LOG(INFO) << "delete temp table :" << m_temp_table_name;
            auto* delete_table_request = brpc::Clone<DeleteTempTableRequest>(request);
            delete_table_request->set_temp_table_name(m_temp_table_name);
            return brpc::SubCall(method, delete_table_request, response->New(), brpc::DELETE_REQUEST | brpc::DELETE_RESPONSE);
        }
        else if (request_name.find("TableRequest") != std::string::npos){
            auto *table_request = brpc::Clone<TableRequest>(request);
            table_request->set_temp_table_name(m_temp_table_name);
            return brpc::SubCall(method, table_request, response->New(), brpc::DELETE_REQUEST | brpc::DELETE_RESPONSE);
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
        else if (response_name.find("TableResponse") != std::string::npos){
            // 1. union -- merge before create temp table
            auto *tableResponse = (TableResponse *) sub_response;
            LOG(INFO) << "response size : " << tableResponse->attr_values_size();
            if (m_is_union) {
                response->MergeFrom(*sub_response);
            }
                // 2. join or leaf-node -- create temp table before join
            else {
                std::string drop_table_sql = "drop table if exists `" + m_temp_table_name + "`;";
                // todo : handle sql execution failure
                execute_non_query_sql(drop_table_sql);
                std::string create_table_sql = "create table `" + m_temp_table_name + "` ( " + tableResponse->attr_meta() + ", " + "index (" + m_index_attr + ")"
                                               " ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
                execute_non_query_sql(create_table_sql);
                if (tableResponse->attr_values_size() == 0) {
                    LOG(ERROR) << "no response!";
                } else {
                    std::string insert_table_sql =
                            "insert into `" + m_temp_table_name + "` values (" + tableResponse->attr_values(0) + ")";
                    for (int i = 1; i < tableResponse->attr_values_size(); ++i) {
                        insert_table_sql += ", (" + tableResponse->attr_values(i) + ")";
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


class DDBServiceImpl : public DDBService {
public:
    void RequestTable(::google::protobuf::RpcController *controller, const ::TableRequest *request,
                      ::TableResponse *response, ::google::protobuf::Closure *done) override;

    void LoadTable(::google::protobuf::RpcController *controller, const ::LoadTableRequest *request,
                   ::LoadTableResponse *response, ::google::protobuf::Closure *done) override;

    void DeleteTable(::google::protobuf::RpcController *controller, const ::DeleteTempTableRequest *request,
                     ::DeleteTempTableResponse *response, ::google::protobuf::Closure *done) override;

    void ExecuteNonQuerySQL(::google::protobuf::RpcController *controller, const ::ExecuteNonQuerySQLRequest *request,
                            ::ExecuteNonQuerySQLResponse *response, ::google::protobuf::Closure *done) override;

    void ExecuteQuerySQL(::google::protobuf::RpcController* controller,
                       const ::ExecuteQuerySQLRequest* request,
                       ::ExecuteQuerySQLResponse* response,
                       ::google::protobuf::Closure* done) override;                        
};


void DDBServiceImpl::RequestTable(::google::protobuf::RpcController *controller, const ::TableRequest *request,
                                  ::TableResponse *response, ::google::protobuf::Closure *done) {

    // RAII : make sure that done->run() will be executed to send response...
    brpc::ClosureGuard done_guard(done);
    auto *cntl =
            dynamic_cast<brpc::Controller *>(controller);
//        response->
    // 1.read meta of requested table
    //int unique_query_id = request->unique_query_id();
    const std::string& temp_table_name = request->temp_table_name();
    LOG(INFO) << "received request for " << temp_table_name;

    temp_table tb = read_temp_table_meta(temp_table_name);
    if (tb.ret_code == -1)
    {
        LOG(INFO) << "no valid information for: " << temp_table_name;
        cntl->SetFailed("no valid information for: " + temp_table_name);
        return;
    }
    

    // 2.for leaf -> executing sql
    if (tb.type == LEAF) {
        std::string table_name = tb.children.cbegin()->first;

        LOG(INFO) << temp_table_name << " is a leaf node, begin to query " << table_name;

        if (tb.project_expr.empty()) {
            tb.project_expr = "*";
        }
        std::string sql = "select " + tb.project_expr + " from `" + table_name + "`";
        if (!tb.select_expr.empty()) {
            sql += " where " + tb.select_expr;
        }
        sql += ";";
        // assume the pattern of table_name is : siteX_original_table_name
        std::string original_table_name = table_name.substr(table_name.find('_') + 1);
        int count = execute_query_sql(sql, original_table_name, response, LEAF);
        if (count <= 0) {
            cntl->SetFailed("empty result for sql: " + sql);
        }
        return;
    }
        // 3.for non-leaf -> cascading rpc request(parallel channel)
    else {

        LOG(INFO) << temp_table_name << " is a non-leaf node, begin to request it children";

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

        // 3.1 request table from children
        std::string first_child_temp_table_name = tb.children.cbegin()->first;
        // 3.1.1 get join attr for index
        std::unordered_map<std::string, std::string> join_attrs;
        if (!tb.is_union){
            // left table
            std::string temp = tb.join_expr.substr(0, tb.join_expr.find(' '));
            int index = temp.find('.');
            std::string key = temp.substr(0, index);
            std::string value = temp.substr(index + 1);
            join_attrs[key] = value;
            // right table
            temp = tb.join_expr.substr(tb.join_expr.find_last_of(' ') + 1);
            index = temp.find('.');
            key = temp.substr(0, index);
            value = temp.substr(index + 1);
            join_attrs[key] = value;
        }
        std::vector<std::string> child_temp_table_names;
        for (auto iter = tb.children.cbegin(); iter != tb.children.cend(); iter++) {
            LOG(INFO) << temp_table_name << " ---> " << iter->first;
            child_temp_table_names.emplace_back(iter->first);
            MyCallMapper *mapper = new MyCallMapper(iter->first, true);
            MyResponseMerger *merger;
            if (tb.is_union){
                merger = new MyResponseMerger(tb.is_union, iter->first);
            } else{
                merger = new MyResponseMerger(tb.is_union, iter->first, join_attrs[iter->first]);
            }
            brpc::Channel *sub_channel = new brpc::Channel;
            std::string server_addr = iter->second + ":8000";
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
            DDBService_Stub stub(&channel);
            TableRequest sub_request;
            TableResponse sub_response;
            brpc::Controller sub_cntl;
            DeleteTempTableRequest delete_request;
            DeleteTempTableResponse delete_response;

            stub.RequestTable(&sub_cntl, &sub_request, &sub_response, NULL);
            bool failed = false;
            if (!sub_cntl.Failed()) {
                // 3.1.1 write query processing statistics to etcd
                std::vector<long> latencies;
                std::vector<long> communication_costs;
                for (int i = 0; i < sub_cntl.sub_count(); i++) {
                    if (!sub_cntl.sub(i)) {
                        LOG(ERROR) << "a controller was lost!";
                        cntl->SetFailed("a controller was lost!");
                        failed = true;
                        break;
                    }
                    if (sub_cntl.sub(i)->Failed()) {
                        LOG(ERROR) << "rpc for channel " << sub_cntl.sub(i)->remote_side().ip << " Failed!";
                        LOG(ERROR) << sub_cntl.sub(i)->ErrorText();
                        std::string s = butil::ip2str(sub_cntl.sub(i)->remote_side().ip).c_str();
                        cntl->SetFailed("rpc for channel " + s + " Failed!");
                        failed = true;
                        break;
                    }
                    latencies.emplace_back(sub_cntl.sub(i)->latency_us());
                    if (sub_cntl.sub(i)->local_side().ip != sub_cntl.sub(i)->remote_side().ip) {
                        communication_costs.emplace_back(sub_cntl.sub(i)->response()->ByteSize());
                    } else {
                        communication_costs.emplace_back(0L);
                    }
                }
                sub_cntl.Reset();

                if(!failed){
                    // todo(1203 by swh) : delete statistics after query
                    write_query_processing_statistics_to_etcd(child_temp_table_names, latencies, communication_costs);
                }

                // 3.2 handle children response
                // 3.2.1 union
                if (tb.is_union) {
                    LOG(INFO) << temp_table_name << "'s combine operator is union, begin to create result temp table named first child : " << first_child_temp_table_name;
                    // 3.2.1.1 create temp table
                    // for union, create temp table after response merge, just use the first child temp_table_name as temp table name
                    // todo(11/14 by swh) : when to delete temp table?
                    std::string drop_table_sql = "drop table if exists `" + first_child_temp_table_name + "`;";
                    // todo : handle sql execution failure
                    execute_non_query_sql(drop_table_sql);
                    std::string create_table_sql =
                            "create table `" + first_child_temp_table_name + "` ( " + sub_response.attr_meta() +
                            " ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
                    execute_non_query_sql(create_table_sql);
                    if (sub_response.attr_values_size() == 0) {
                        LOG(ERROR) << "no response!";
                    } else {
                        std::string insert_table_sql =
                                "insert into `" + first_child_temp_table_name + "` values (" +
                                sub_response.attr_values(0) + ")";
                        for (int i = 1; i < sub_response.attr_values_size(); ++i) {
                            insert_table_sql += ", (" + sub_response.attr_values(i) + ")";
                        }
                        insert_table_sql += ";";
                        execute_non_query_sql(insert_table_sql);
                    }

                    // 3.2.1.2 single op -- project at most
                    if (tb.project_expr.empty()) {
                        tb.project_expr = "*";
                    }
                    // LOG(INFO) << "project of " << temp_table_name << ": " << tb.project_expr;
                    std::string sql = "select " + tb.project_expr + " from " + first_child_temp_table_name;
                    if (!tb.select_expr.empty()) {
                        sql += " where " + tb.select_expr;
                    }
                    sql += ";";
                    // LOG(INFO) << "sql before execute of  " << temp_table_name << ": " << sql; 
                    int count = execute_query_sql(sql, first_child_temp_table_name, response, NON_LEAF);
                    if (count <= 0) {
                        cntl->SetFailed("empty result for sql: " + sql);
                    }
                }
                    // 3.2.2 join
                else {

                    LOG(INFO) << temp_table_name << "'s combine operator is join, begin to execute join in mysql";

                    if (tb.project_expr.empty()) {
                        tb.project_expr = "*";
                    }
                    std::string sql = "select " + tb.project_expr + " from ";
                    auto iter = tb.children.cbegin();
                    sql += iter->first + " join ";
                    iter++;
                    sql += iter->first;
                    // sql += " on " + tb.join_expr;
                    // for (auto iter = tb.children.cbegin(); iter != tb.children.cend(); iter++) {
                    //     sql += iter->first + ", ";
                    // }
                    // sql = sql.substr(0, sql.length() - 2);
                    // sql += " where " + tb.join_expr;

                    // std::vector<std::string> ts = s_split(tb.join_expr, "(\s?)=(\s?)");

                    int index = tb.join_expr.find('=');
                    std::string s1 = tb.join_expr.substr(0, index);
                    std::string s2 = tb.join_expr.substr(index + 1);
                    trim(s1);
                    trim(s2);
                    std::string attr1 = s1.substr(s1.find('.') + 1);
                    std::string attr2 = s2.substr(s2.find('.') + 1);
                    if (attr1 == attr2)
                    {
                        sql += " using(" + attr1 + ")";
                    }
                    else{
                        sql += " on " + tb.join_expr;
                    }
                    

                    if (!tb.select_expr.empty()) {
                        sql += " where " + tb.select_expr;
                    }
                    sql += ";";
                    int count = execute_query_sql(sql, first_child_temp_table_name, response, NON_LEAF);
                    if (count <= 0) {
                        cntl->SetFailed("empty result for sql: " + sql);
                    }
                }

                stub.DeleteTable(&sub_cntl, &delete_request, &delete_response, NULL);

            } else {
                LOG(ERROR) << "cascading rpc request failed!";
                LOG(ERROR) << sub_cntl.ErrorText();
                cntl->SetFailed("cascading rpc request failed!");
                return;
            }
        }
    }
}


void DDBServiceImpl::LoadTable(::google::protobuf::RpcController *controller, const ::LoadTableRequest *request,
                               ::LoadTableResponse *response, ::google::protobuf::Closure *done) {
    // RAII : make sure that done->run() will be executed to send response...
    brpc::ClosureGuard done_guard(done);
    auto *cntl =
            dynamic_cast<brpc::Controller *>(controller);
    const std::string& table_name = request->table_name();
    const std::string& attr_meta = request->attr_meta();

    std::string create_table_sql = "create table if not exists `" + table_name + "` ( " + attr_meta +
                                   " ) ENGINE=InnoDB DEFAULT CHARSET=utf8;";
    execute_non_query_sql(create_table_sql);
    if (request->attr_values_size() == 0) {
        LOG(ERROR) << "no data!";
        response->set_result("zero rows to be inserted!");
    } else {
        std::string insert_table_sql =
                "insert into `" + table_name + "` values (" + request->attr_values(0) + ")";
        for (int i = 1; i < request->attr_values_size(); ++i) {
            insert_table_sql += ", (" + request->attr_values(i) + ")";
        }
        insert_table_sql += ";";
        execute_non_query_sql(insert_table_sql);
        response->set_result("succeed!");
    }
}

void DDBServiceImpl::DeleteTable(::google::protobuf::RpcController *controller, const ::DeleteTempTableRequest *request,
                                 ::DeleteTempTableResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    auto *cntl = dynamic_cast<brpc::Controller *>(controller);
    std::string temp_table_name = request->temp_table_name();
    std::string sql = "drop table if exists `" + temp_table_name + "`;";
    execute_non_query_sql(sql);
    response->set_result("succeed!");
}

void DDBServiceImpl::ExecuteNonQuerySQL(::google::protobuf::RpcController *controller,
                                        const ::ExecuteNonQuerySQLRequest *request,
                                        ::ExecuteNonQuerySQLResponse *response, ::google::protobuf::Closure *done) {
    brpc::ClosureGuard done_guard(done);
    auto *cntl = dynamic_cast<brpc::Controller *>(controller);
    const std::string& sql = request->sql();
    execute_non_query_sql(sql);
    response->set_result("succeed!");
}

void DDBServiceImpl::ExecuteQuerySQL(::google::protobuf::RpcController* controller, const ::ExecuteQuerySQLRequest* request,
                    ::ExecuteQuerySQLResponse* response, ::google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    auto *cntl = dynamic_cast<brpc::Controller *>(controller);
    const std::string& sql = request->sql();
    execute_query_sql(sql, response);
}


int main(int argc, char *argv[]) {
    // Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

    // Generally you only need one Server.
    brpc::Server server;

    // Instance of your service.
    DDBServiceImpl ddb_service_impl;

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
        LOG(ERROR) << "Fail to start DDBServer";
        return -1;
    }

    // Wait until Ctrl-C is pressed, then Stop() and Join() the server.
    server.RunUntilAskedToQuit();
    return 0;
}
