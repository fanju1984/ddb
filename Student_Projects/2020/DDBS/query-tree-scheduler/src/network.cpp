#include "brpc/channel.h"
#include "butil/logging.h"

#include "json.hpp"
#include "base64.h"

#include <string>
#include <vector>
#include <unordered_map>

#include "ddb.pb.h"

DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 3000000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_string(http_protocol, "http", "Client-side protocol");
// DEFINE_string(temp_table, "1-temp-table", "Temp table name.");
DEFINE_string(rpc_protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");



const std::string ETCD_PUT_URL = "http://127.0.0.1:2379/v3/kv/put";
const std::string ETCD_GET_URL = "http://127.0.0.1:2379/v3/kv/range";
const std::string ETCD_DELETE_URL = "http://127.0.0.1:2379/v3/kv/deleterange";

const std::string QUERY_PROCESSING_STATISTICS_PREFIX = "/query_processing_statistics";

// etcd
int write_kv_to_etcd(const std::string& key, const std::string& value){
    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    LOG(INFO) << "put <" << key << "," << value << ">";
    nlohmann::json j;
    j["key"] = base64_encode(key);
    j["value"] = base64_encode(value);
    cntl.http_request().uri() = ETCD_PUT_URL;
    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.request_attachment().append(j.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
    if(cntl.Failed()){
        LOG(ERROR) << cntl.ErrorText() << std::endl;
        return -1;
    }
    return 0;
}
int write_map_to_etcd(const std::map<std::string, std::string>& mp){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    for (auto iter = mp.cbegin(); iter != mp.cend(); iter++) {
        std::string key = iter->first;
        std::string value = iter->second;
        LOG(INFO) << "put <" << key << "," << value << ">";
        nlohmann::json j;
        j["key"] = base64_encode(key);
        j["value"] = base64_encode(value);
        cntl.http_request().uri() = ETCD_PUT_URL;
        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if(cntl.Failed()){
            LOG(ERROR) << cntl.ErrorText() << std::endl;
            return -1;
        }
        cntl.Reset();
    }
    return 0;
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
nlohmann::json read_from_etcd(const std::string &key, bool prefix) {

    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    nlohmann::json j;

    if (channel.Init(ETCD_GET_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return j;
    }

    brpc::Controller cntl;

    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.http_request().uri() = ETCD_GET_URL;

    j["key"] = base64_encode(key);
    if (prefix) {
        j["range_end"] = base64_encode(get_range_end_for_prefix(key));
    }
    cntl.request_attachment().append(j.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
    if (cntl.Failed()) {
        LOG(ERROR) << cntl.ErrorText();
        return NULL;
    }
    if (cntl.Failed()) {
        LOG(ERROR) << cntl.ErrorText();
        return NULL;
    }
    j.clear();
    std::string temp = cntl.response_attachment().to_string();
    // cntl.Reset();
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

std::string read_from_etcd_by_key(const std::string& key){

    nlohmann::json j = read_from_etcd(key, false);
    if (j != NULL) {
        std::string temp = j["count"];
        int count = std::stoi(temp);
        if (count > 0) {
            temp = j["kvs"][0]["value"];
            return base64_decode(temp);
        }
    }
    return "";
}
std::unordered_map<std::string, std::string> read_from_etcd_by_prefix(const std::string& prefix){
    nlohmann::json j = read_from_etcd(prefix, true);
    std::unordered_map<std::string, std::string> mp;
    if (j != NULL) {
        std::string temp = j["count"];
        int count = std::stoi(temp);
        if (count > 0) {
            std::string temp2;
            for (int i = 0; i < count; ++i) {
                temp = j["kvs"][i]["key"];
                temp2 = j["kvs"][i]["value"];
                mp[base64_decode(temp)] = base64_decode(temp2);
            }
        }
    }
    return mp;
}

int delete_from_etcd(const std::string& key, bool prefix){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_DELETE_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl.http_request().uri() = ETCD_DELETE_URL;

    nlohmann::json j;
    j["key"] = base64_encode(key);
    if (prefix) {
        j["range_end"] = base64_encode(get_range_end_for_prefix(key));
    }
    cntl.request_attachment().append(j.dump());
    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
    if (cntl.Failed()) {
        LOG(ERROR) << cntl.ErrorText();
        return -1;
    }
    // j = nlohmann::json::parse(cntl.response_attachment().to_string());
    return 0;
}

int delete_from_etcd_by_key(const std::string& key){
    return delete_from_etcd(key, false);
}
int delete_from_etcd_by_prefix(const std::string& prefix){
    return delete_from_etcd(prefix, true);
}
std::map<std::string, std::string> read_map_from_etcd(const std::vector<std::string>& keys){

    std::map<std::string, std::string> mp;

    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    nlohmann::json j;

    if (channel.Init(ETCD_GET_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return mp;
    }

    brpc::Controller cntl;

    for(const std::string& key : keys){
        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.http_request().uri() = ETCD_GET_URL;

        j["key"] = base64_encode(key);
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if (cntl.Failed()) {
            LOG(ERROR) << cntl.ErrorText();
            return mp;
        }

        std::string temp = cntl.response_attachment().to_string();
    //    LOG(INFO) << "read " << key << " from etcd, receives " << temp;
        j = nlohmann::json::parse(temp);
        if (j["count"].type_name() != "null"){
            temp = j["count"];
            if (std::stoi(temp) != 0){
                temp = j["kvs"][0]["value"];
                mp[key] =  base64_decode(temp);
            }
        }
        
        cntl.Reset();   
    }
    return mp;
}



// rpc
int load_table(const std::string& host, const std::string& table_name, const std::string& attr_meta, const std::vector<std::string>& attr_values){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_rpc_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;
    if (channel.Init(host.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    DDBService_Stub stub(&channel);

    // async client
    // todo : add multi-thread implementation
    {
        auto* cntl = new brpc::Controller;
        auto* response = new LoadTableResponse;

        LoadTableRequest request;
        request.set_table_name(table_name);
        request.set_attr_meta(attr_meta);
        for(const std::string& attr_value : attr_values){
            request.add_attr_values(attr_value);
        }

//        google::protobuf::Closure* done = brpc::NewCallback(&HandleLoadTableResponse, cntl, response);

        stub.LoadTable(cntl, &request, response, nullptr);
        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
        LOG(INFO) << "Result: " << response->result();
    }

    return 0;
}
std::vector<std::string> request_table(const std::string& temp_table_name){
    std::vector<std::string> result;
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_rpc_protocol;
    options.connection_type = FLAGS_connection_type;
    options.timeout_ms = FLAGS_timeout_ms/*milliseconds*/;
    options.max_retry = FLAGS_max_retry;
    if (channel.Init(FLAGS_server.c_str(), FLAGS_load_balancer.c_str(), &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return result;
    }

    // Normally, you should not call a Channel directly, but instead construct
    // a stub Service wrapping it. stub can be shared by all threads as well.
    DDBService_Stub stub(&channel);


    // sync client
    // todo : add multi-thread implementation
    {
        brpc::Controller* cntl = new brpc::Controller;
        TableResponse* response = new TableResponse;

        TableRequest request;
        request.set_temp_table_name(temp_table_name);

//        google::protobuf::Closure* done = brpc::NewCallback(
//                &HandleRequestTableResponse, cntl, response);

        stub.RequestTable(cntl, &request, response, nullptr);

        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return result;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
//        std::cout << "Result meta: " << response->attr_meta() << std::endl;
//        std::cout << "Result: " << std::endl;
        // result.emplace_back(response->attr_meta());
        for (int i = 0; i < response->attr_values_size(); ++i) {
//            std::cout << response->attr_values(i) << std::endl;
            result.emplace_back(response->attr_values(i));
        }

        write_kv_to_etcd(temp_table_name + ".latency", std::to_string(cntl->latency_us()));
        write_kv_to_etcd(temp_table_name + ".communication-cost", std::to_string(0));

                // delete root temp table
        cntl->Reset();
        DeleteTempTableResponse* delete_response = new DeleteTempTableResponse;
        DeleteTempTableRequest delete_request;
        delete_request.set_temp_table_name(temp_table_name);
        stub.DeleteTable(cntl, &delete_request, delete_response, NULL);
        // todo(1203 by swh) : handle statistics(show, delete)

        std::cout << "Result count: " << response->attr_values().size() << std::endl;
    }

    return result;
}
std::map<std::string, std::string> get_request_statistics(const std::vector<std::string>& temp_table_names){
    std::map<std::string, std::string> mp;

    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_GET_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return mp;
    }

    brpc::Controller cntl;
    nlohmann::json j;
    for(const std::string& temp_table_name : temp_table_names){

        // LOG(INFO) << "read latency and communication-cost of " << temp_table_name;
        std::string lm;

        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.http_request().uri() = ETCD_GET_URL;

        // latency
        j["key"] = base64_encode(temp_table_name + ".latency");
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if (cntl.Failed()) {
            LOG(ERROR) << cntl.ErrorText();
            continue;
        }
        std::string temp = cntl.response_attachment().to_string();
        j = nlohmann::json::parse(temp);
        // LOG(INFO) << "latency of " << temp_table_name << ": ";
        // LOG(INFO) << j.dump();
        if (j["count"].type_name() != "null"){
            temp = j["count"];
            if (std::stoi(temp) != 0){
                temp = j["kvs"][0]["value"];
                lm = base64_decode(temp);
            }
        }

        cntl.Reset();

        cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl.http_request().uri() = ETCD_GET_URL;

        // communication cost
        j["key"] = base64_encode(temp_table_name + ".communication-cost");
        cntl.request_attachment().append(j.dump());
        channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
        if (cntl.Failed()) {
            LOG(ERROR) << cntl.ErrorText();
            continue;
        }
        temp = cntl.response_attachment().to_string();
        j = nlohmann::json::parse(temp);

        // LOG(INFO) << "communication-cost of " << temp_table_name << ": ";
        // LOG(INFO) << j.dump();

        if (j["count"].type_name() != "null"){
            temp = j["count"];
            if (std::stoi(temp) != 0){
                temp = j["kvs"][0]["value"];
                lm += "," + base64_decode(temp);
            }
        }
        mp[temp_table_name] = lm;
        cntl.Reset();
        j.clear();
    }
    return mp;
}

std::string execute_non_query_sql(const std::string& ip, const std::string& sql){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init((ip + ":8000").c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return "Fail to initialize channel";
    }

    brpc::Controller cntl;
    ExecuteNonQuerySQLRequest request;
    ExecuteNonQuerySQLResponse response;
    request.set_sql(sql);
    DDBService_Stub stub(&channel);
    stub.ExecuteNonQuerySQL(&cntl, &request, &response, nullptr);
    if (cntl.Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl.ErrorText();
        return cntl.ErrorText();
    }

    return response.result();
}


std::vector<std::string> execute_query_sql(const std::string& ip, const std::string& sql){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_http_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    std::vector<std::string> v;

    if (channel.Init((ip + ":8000").c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return v;
    }

    brpc::Controller cntl;
    ExecuteQuerySQLRequest request;
    ExecuteQuerySQLResponse response;
    request.set_sql(sql);
    DDBService_Stub stub(&channel);
    stub.ExecuteQuerySQL(&cntl, &request, &response, nullptr);
    if (cntl.Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl.ErrorText();
        return v;
    }
    v.emplace_back(response.attr_meta());
    for (int i = 0; i < response.attr_values_size(); ++i) {
        v.emplace_back(response.attr_values(i));
    }

    return v;
}