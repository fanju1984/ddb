/* network.cpp

    RPCs for query-plan scheduling.

    Created by khaiwang

    2021/11/16
*/

#include "network.h"
#include "ddb.pb.h"
#include "butil/logging.h"
#include "base64.h"
#include "json.hpp"

#include <string>
#include <vector>

#ifndef FLAGS
#define FLAGS
DEFINE_string(http_protocol, "http", "http protocal type for etcd");
DEFINE_string(rpc_protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_string(etcd_put_url, "http://127.0.0.1:2379/v3/kv/put", "IP address of etcd put method");
DEFINE_string(etcd_get_url, "http://127.0.0.1:2379/v3/kv/range", "IP address of etcd get method");
DEFINE_string(etcd_delete_url, "http://127.0.0.1:2379/v3/kv/deleterange", "IP address of etcd delete method");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 30000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");

const std::string ETCD_PUT_URL = "http://127.0.0.1:2379/v3/kv/put";
const std::string ETCD_GET_URL = "http://127.0.0.1:2379/v3/kv/range";
const std::string ETCD_DELETE_URL = "http://127.0.0.1:2379/v3/kv/deleterange";
#endif

/** write a key-value pair to etcd, kv needs to be transformed into json
    arguments:
    @param[in] key:     input key, in string format
    @param[in] value:   input value, in string format
*/
int write_kv_to_etcd(const std::string &key, const std::string &value) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(FLAGS_etcd_put_url.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }
    nlohmann::json kv;

    kv["key"]  = base64_encode(key);
    kv["value"] = base64_encode(value);

    brpc::Controller cntl;
    cntl.http_request().uri() = ETCD_PUT_URL;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(kv.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);

    if(cntl.Failed()){
        LOG(ERROR) << cntl.ErrorText() << std::endl;
        return -1;
    }
    return 0;
}

/** write a map to etcd, the format of map: string->string
 *  arguments:
 *  @param map: map of kv pairs(type string)
*/ 
int write_map_to_etcd(const std::map<std::string, std::string> &map) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(FLAGS_etcd_put_url.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }
    
    brpc::Controller cntl;

    for(auto iter = map.begin(); iter != map.end(); iter++){
        nlohmann::json kv;
        kv["key"]  = base64_encode(iter->first);
        kv["value"] = base64_encode(iter->second);
        
        cntl.http_request().uri() = ETCD_PUT_URL;  // 设置为待访问的URL
        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.request_attachment().append(kv.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);

        if(cntl.Failed()){
            LOG(ERROR) << cntl.ErrorText() << std::endl;
            return -1;
        }
        cntl.Reset();
    }
}

/** Read a value pair from etcd by key, string kv needs to be transformed from json
    arguments:
    @param[in] key:     input key, in string format
    @param[out] value:   output value, in string format
*/
std::string read_kv_from_etcd(const std::string &key) {
    nlohmann::json kv = read_from_etcd(key);
    if(kv != NULL) {
        std::string temp = kv["count"];
        int count = std::stoi(temp);
        if(count > 0) {
            std::string result = kv["kvs"][0]["value"];
            // LOG(INFO) << "read " << key << " from etcd, receives " << result;
            return base64_decode(result);
        }
    }
    return "";
}

/** Read a struct from etcd by key
    arguments:
    @param[in] key:     input key, in string format
    @param[out] value:   output structure, in json format
*/
nlohmann::json read_from_etcd(const std::string &key) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(FLAGS_etcd_get_url.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
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
    // LOG(INFO) << "read " << key << " from etcd, receives " << temp;
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

/** delete a kv from etcd by key
    arguments:
    @param[in] key:     input key, in string format
*/
int delete_kv_from_etcd(const std::string &key) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(FLAGS_etcd_delete_url.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }
    nlohmann::json kv;

    kv["key"]  = base64_encode(key);
    
    brpc::Controller cntl;
    cntl.http_request().uri() = ETCD_DELETE_URL;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(kv.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);

    if(cntl.Failed()){
        LOG(ERROR) << cntl.ErrorText() << std::endl;
        return NULL;
    }
}

/** Write a struct kv to etcd
    arguments:
    @param[in] key:     input key, in string format
    @param[in] value:   input structure, in json format
*/
int write_to_etcd(const std::string &key, const nlohmann::json &value) {
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    //options.connection_type = FLAGS_connection_type;
    if (channel.Init(FLAGS_etcd_put_url.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }
    nlohmann::json kv;
    // LOG(INFO) << "write " << key << " : " << value.dump();
    kv["key"]  = base64_encode(key);
    kv["value"] = base64_encode(value.dump());
    //kv["key"] = key;
    // kv.merge_patch(value);
    // std::cout << "kv to write:" << std::endl;
    // std::cout << kv.dump() << std::endl;

    brpc::Controller cntl;
    cntl.http_request().uri() = ETCD_PUT_URL;  // 设置为待访问的URL
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(kv.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL/*done*/);

    if(cntl.Failed()){
        LOG(ERROR) << cntl.ErrorText() << std::endl;
        return -1;
    }
    return 0;
}

// int main(int argc, char* argv[]){

// // Parse gflags. We recommend you to use gflags as well.
//     GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
//     LOG(INFO) << "Init etcd service";
//     LOG(INFO) << "Delete tables";
//     LOG(INFO) << "Delete sites";
//     delete_kv_from_etcd("tables");
//     delete_kv_from_etcd("sites");
// }

// int import_data(const std::string& host, const std::string& table_name, 
//                 const std::string& table_fields, const std::vector<std::string>& table_values){
    
//     brpc::Channel channel;

//     brpc::ChannelOptions options;
//     options.protocol = FLAGS_rpc_protocol;
//     options.connection_type = FLAGS_connection_type;
//     options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
//     options.max_retry = FLAGS_max_retry;
//     if (channel.Init(host.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
//         LOG(ERROR) << "Fail to initialize channel";
//         return -1;
//     }

//     sql_service_Stub stub(&channel);

    
//     brpc::Controller* cntl = new brpc::Controller;
//     ImportDataResponse* response = new ImportDataResponse;

//     ImportDataRequest request;
//     request.set_table_name(table_name);
//     request.set_table_fields(table_fields);
//     for(const std::string& table_value : table_values){
//         request.add_table_values(table_value);
//     }

// //        google::protobuf::Closure* done = brpc::NewCallback(&HandleLoadTableResponse, cntl, response);

//     stub.ImportData(cntl, &request, response, nullptr);
//     if (cntl->Failed()) {
//         LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
//         return -1;
//     }
//     LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
//     LOG(INFO) << "Result: " << response->result();
    
// }

// std::vector<std::string> request_node(const std::string& node_name) {
//     brpc::Channel channel;
//     std::vector<std::string> result;


//     brpc::ChannelOptions options;
//     options.protocol = FLAGS_rpc_protocol;
//     options.connection_type = FLAGS_connection_type;
//     options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
//     options.max_retry = FLAGS_max_retry;
//     if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
//         LOG(ERROR) << "Fail to initialize channel";
//         return result;
//     }

//     sql_service_Stub stub(&channel);

    
//     brpc::Controller* cntl = new brpc::Controller;
//     NodeResponse* response = new NodeResponse;

//     NodeRequest request;
//     request.set_temp_table_name(node_name);

//     stub.RequestNode(cntl, &request, response, nullptr);
//     if (cntl->Failed()) {
//         LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
//         return result;
//     }
//     LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
//     for (int i = 0; i < response->table_values_size(); ++i) {
//         result.push_back(response->table_values(i));
//     }

//     cntl->Reset();
//     DeleteTempTableResponse* delete_response = new DeleteTempTableResponse;
//     DeleteTempTableRequest delete_request;
//     delete_request.set_temp_table_name(node_name);
//     stub.DeleteTable(cntl, &delete_request, delete_response, NULL);
//     std::cout << "Result count: " << response->table_values().size() << std::endl;

// }

// std::string execute_non_query_sql(const std::string& ip, const std::string& sql) {
//      brpc::Channel channel;
//     brpc::ChannelOptions options;
//     options.protocol = FLAGS_http_protocol;
//     options.timeout_ms = FLAGS_timeout_ms;
//     options.max_retry = FLAGS_max_retry;

//     if (channel.Init((ip + ":8000").c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
//         LOG(ERROR) << "Fail to initialize channel";
//         return "Fail to initialize channel";
//     }

//     brpc::Controller *cntl;
//     ExecuteNonQuerySQLRequest request;
//     ExecuteNonQuerySQLResponse response;
//     request.set_sql_query(sql);
//     sql_service_Stub stub(&channel);
//     stub.ExecuteNonQuerySQL(cntl, &request, &response, nullptr);
//     if (cntl->Failed()) {
//         LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
//         return cntl->ErrorText();
//     }

//     return response.result();
// }

// std::vector<std::string> execute_query_sql(const std::string& ip, const std::string& sql) {
//     brpc::Channel channel;
//     brpc::ChannelOptions options;
//     options.protocol = FLAGS_http_protocol;
//     options.timeout_ms = FLAGS_timeout_ms;
//     options.max_retry = FLAGS_max_retry;

//     std::vector<std::string> v;

//     if (channel.Init((ip + ":8000").c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
//         LOG(ERROR) << "Fail to initialize channel";
//         return v;
//     }

//     brpc::Controller *cntl;
//     ExecuteQuerySQLRequest *request;
//     ExecuteQuerySQLResponse *response;
//     request->set_sql(sql);
//     sql_service_Stub stub(&channel);
//     stub.ExecuteQuerySQL(cntl, request, response, nullptr);
//     if (cntl->Failed()) {
//         LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
//         return v;
//     }
//     v.push_back(response->table_fields());
//     for (int i = 0; i < response->table_values_size(); ++i) {
//         v.push_back(response->table_values(i));
//     }

//     return v;
// }