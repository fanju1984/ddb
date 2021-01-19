#include <gflags/gflags.h>
#include "brpc/channel.h"
#include "butil/logging.h"

#include <iostream>
#include <string>
#include <vector>
#include "ddb.pb.h"

DEFINE_string(temp_table, "1-temp-table", "Temp table name.");
DEFINE_string(protocol, "baidu_std", "Protocol type. Defined in src/brpc/options.proto");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
DEFINE_string(server, "0.0.0.0:8000", "IP Address of server");
DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 300000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_int32(interval_ms, 1000, "Milliseconds between consecutive requests");


void HandleRequestTableResponse(
        brpc::Controller* cntl,
        TableResponse* response) {
    // std::unique_ptr makes sure cntl/response will be deleted before returning.
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    std::unique_ptr<TableResponse> response_guard(response);

    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        return;
    }
    LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
    std::cout << "Result meta: " << response->attr_meta() << std::endl;
    std::cout << "Result: " << response->attr_values().size() << std::endl;
    for (int i = 0; i < response->attr_values_size(); ++i) {
        std::cout << response->attr_values(i) << std::endl;
    }
}

void HandleLoadTableResponse(brpc::Controller* cntl, LoadTableResponse* response){
    // std::unique_ptr makes sure cntl/response will be deleted before returning.
    std::unique_ptr<brpc::Controller> cntl_guard(cntl);
    std::unique_ptr<LoadTableResponse> response_guard(response);

    if (cntl->Failed()) {
        LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
        return;
    }
    LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
    LOG(INFO) << "Result: " << response->result();
}

int load_table(std::string host, std::string table_name, std::string attr_meta, std::vector<std::string> attr_values){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
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

int request_table(std::string temp_table_name, std::string host = "127.0.0.1:8000"){
    // A Channel represents a communication line to a Server. Notice that
    // Channel is thread-safe and can be shared by all threads in your program.
    brpc::Channel channel;

    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
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
        brpc::Controller* cntl = new brpc::Controller;
        TableResponse* response = new TableResponse;

        TableRequest request;
        request.set_temp_table_name(temp_table_name);

//        google::protobuf::Closure* done = brpc::NewCallback(
//                &HandleRequestTableResponse, cntl, response);

        stub.RequestTable(cntl, &request, response, NULL);

        if (cntl->Failed()) {
            LOG(WARNING) << "Some site was down, " << cntl->ErrorText();
            return -1;
        }
        LOG(INFO) << "Received response from " << cntl->remote_side() << "\nLatency(us): " << cntl->latency_us();
        std::cout << "Result meta: " << response->attr_meta() << std::endl;
        std::cout << "Result count: " << response->attr_values().size() << std::endl;
        for (int i = 0; i < response->attr_values_size(); ++i) {
            std::cout << response->attr_values(i) << std::endl;
        }

        // delete root temp table
        cntl->Reset();
        DeleteTempTableResponse* delete_response = new DeleteTempTableResponse;
        DeleteTempTableRequest delete_request;
        delete_request.set_temp_table_name(temp_table_name);
        stub.DeleteTable(cntl, &delete_request, delete_response, NULL);
        // todo(1203 by swh) : handle statistics(show, delete)

        std::cout << "Result count: " << response->attr_values().size() << std::endl;
    }


    return 0;
}

int main(int argc, char* argv[]){

// Parse gflags. We recommend you to use gflags as well.
    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);

//    std::string host = "127.0.0.1:8000";
//    std::string table_name = "test1";
//    std::string attr_meta = "`id` int(11) not null, `name` varchar(20) not null";
//    std::vector<std::string> attr_values;
//    attr_values.emplace_back("1, 'a'");
//    attr_values.emplace_back("2, 'b'");
//    attr_values.emplace_back("3, 'c'");
//
//    return load_table(host, table_name, attr_meta, attr_values);

    return request_table("query_0_0_temp_table", "127.0.0.1:8000");
}