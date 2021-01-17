#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/channel.h>
#include <brpc/parallel_channel.h>
#include "base64.h";
#include "json.hpp";
#include "query.pb.h"

#include <iostream>
#include <fstream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>


DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 2000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)"); 
DEFINE_string(protocol, "http", "Client-side protocol");
DEFINE_string(connection_type, "", "Connection type. Available values: single, pooled, short");
// async sync
// push pull

enum readtype{
    KEY=0,
    PREFIX=1
};

struct fragment
{
    std::string db_name;
    std::string table_name;
    int fragno;
    std::string ip;
    int max_id;
    int min_id;
    std::string type;
};

std::string get_range_end_for_prefix(std::string s){
    for (int i = 0; i < s.length(); i++)
    {
        if(s[i] < 0xff){
            s[i]++;
            return s.substr(0, i + 1);
        }
    }
    return "";
}

nlohmann::json read_from_etcd(brpc::Channel* channel, brpc::Controller* cntl, char* get_url, std::string key, readtype type){


    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl->http_request().uri() = get_url;

    nlohmann::json j;
    j["key"] = base64_encode(key);
    if(type == PREFIX){
        j["range_end"] = base64_encode(get_range_end_for_prefix(key));
    }
    cntl->request_attachment().append(j.dump());
    channel->CallMethod(NULL, cntl, NULL, NULL, NULL);
    if(cntl->Failed()){
        LOG(ERROR) << cntl->ErrorText();
        return j;
    }
    j.clear();
    j = nlohmann::json::parse(cntl->response_attachment().to_string());
    cntl->Reset();
    return j;
    
}

std::vector<fragment> get_fragments(std::string db_name, std::string table_name){

    std::vector<fragment> frags;

    char* get_url = "http://127.0.0.1:2379/v3/kv/range";
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;
    if (channel.Init("http://127.0.0.1:2379", FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return frags;
    }
    brpc::Controller cntl;
    
    std::string temp;
    // 1.fragnum
    std::string prefix = "/" + db_name + "/" + table_name;
    std::string key = prefix + "/fragnum";
    nlohmann::json j = read_from_etcd(&channel, &cntl, get_url, key, KEY);
    temp = j["count"];
    int count = std::stoi(temp);
    if(count == 0){
        LOG(ERROR) << "no fragments for " << db_name << ":" << table_name;
    }
    temp = j["kvs"][0]["value"];
    int fragnum = std::stoi(base64_decode(temp));

    // 2.read fragment info 
    for (int i = 0; i < fragnum; i++)
    {
        key = prefix + "/" + std::to_string(i);
        j = read_from_etcd(&channel, &cntl, get_url, key, PREFIX);
        fragment frag;
        frag.db_name = db_name;
        frag.table_name = table_name;
        frag.fragno = i;

        std::unordered_map<std::string, std::string> mp;
        for (auto iter = j["kvs"].cbegin(); iter != j["kvs"].cend(); iter++)
        {
            mp[(*iter)["key"]] = (*iter)["value"];
        }
        frag.ip = base64_decode(mp[base64_encode(prefix + "/" + std::to_string(i) + "/ip")]);
        frag.type = base64_decode(mp[base64_encode(prefix + "/" + std::to_string(i) + "/type")]);
        frag.max_id = std::stoi(base64_decode(mp[base64_encode(prefix + "/" + std::to_string(i) + "/max_id")]));
        frag.min_id = std::stoi(base64_decode(mp[base64_encode(prefix + "/" + std::to_string(i) + "/min_id")]));
        frags.push_back(frag);
    }
    return frags;
}

struct temptable_users
{
    std::vector<int> ids;
    std::vector<std::string> names;
    std::vector<std::string> fullnames;
    std::vector<std::string> passwords;
};


class UseFieldAsSubRequest : public brpc::CallMapper{
public:
    brpc::SubCall Map(int channel_index, const google::protobuf::MethodDescriptor* method,
                    const google::protobuf::Message* request, google::protobuf::Message* response){
                        QueryUsersRequest* query_request = brpc::Clone<QueryUsersRequest>(request);
                        query_request->add_query(m_query_str);
                        std::cout << "CallMapper :" << m_query_str << std::endl;
                        return brpc::SubCall(method, query_request, response->New(), brpc::DELETE_REQUEST | brpc::DELETE_RESPONSE);
                    }

    void set_query_str(std::string query_str){
        this->m_query_str = query_str;
    }

private:
    std::string m_query_str;
};

enum BinaryOperator{
    UNION, JOIN
};

class MyResponseMerger : public brpc::ResponseMerger{
public: 
    brpc::ResponseMerger::Result Merge(google::protobuf::Message* response, const google::protobuf::Message* sub_response)
    {
        QueryUsersResponse* response1 = (QueryUsersResponse*)response;
        QueryUsersResponse* sub_response1 = (QueryUsersResponse*)sub_response;
        brpc::ResponseMerger::Result result;
        if(m_op == UNION){
            // print temp result
            // auto iter_id = sub_response1->id().cbegin();
            // auto iter_fullname = sub_response1->fullname().cbegin();
            // auto iter_password = sub_response1->password().cbegin();
            std::cout << "---------------------------" << std::endl;
            std::cout << ">> receive data from a site :" << std::endl;
            // // std::cout << sub_response1->id_size() << std::endl;
            // while(iter_id != sub_response1->id().cend() && iter_fullname != sub_response1->fullname().cend() && iter_password != sub_response1->password().cend())
            // {
            //     std::cout << "(" << *iter_id << ", " << *iter_fullname << ", " << *iter_password << ")" << std::endl;
            //     iter_id++;
            //     iter_fullname++;
            //     iter_password++;
            // }
            int count = sub_response1->id_size();
            for (int i = 0; i < count; i++)
            {
                std::cout << "(" << sub_response1->id(i) << ", " << sub_response1->fullname(i) << ", " << sub_response1->password(i) << ")" << std::endl;
            }
            
            std::cout << std::endl;
            response1->MergeFrom(*sub_response1);
            return result;
        }
        else{
            // save temp table
            // m_stat->execute("use test;");

        }
    }

    void set_op(BinaryOperator op){
        this->m_op = op;
    }
    void set_statement(sql::Statement* stat){
        this->m_stat = stat;
    }

private:
    BinaryOperator m_op;
    sql::Statement* m_stat;
};

int main(int argc, char* argv[]){

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // 1.get fragment info for db:table
    std::string db_name = "test";
    std::string table_name = "users";
    std::vector<fragment> frags = get_fragments(db_name, table_name);

    if(frags.empty()){
        LOG(WARNING) << "no available fragments!";
        return -1;
    }

    // 2.generate rpc request
    brpc::ParallelChannel channel;
    brpc::ParallelChannelOptions pchan_options;
    pchan_options.timeout_ms = FLAGS_timeout_ms;
    if(channel.Init(&pchan_options) != 0){
        LOG(ERROR) << "Fail to init ParallelChannel!";
        return -1;
    }
    brpc::ChannelOptions sub_options;
    sub_options.protocol = FLAGS_protocol;
    sub_options.connection_type = FLAGS_connection_type;
    sub_options.max_retry = FLAGS_max_retry;


    int id_to_query = 12;
    std::string query_str = "select id, fullname, password from users where id <= " + std::to_string(id_to_query) + ";";
    std::cout << ">> executing query : " << query_str << std::endl;
    std::cout << std::endl;
    BinaryOperator op;
    op = UNION;
    MyResponseMerger* merger = new MyResponseMerger;
    merger->set_op(op);
    
    for(auto iter = frags.cbegin(); iter != frags.cend(); iter++){
        if(id_to_query >= iter->min_id){
            UseFieldAsSubRequest* mapper = new UseFieldAsSubRequest;
            mapper->set_query_str(query_str);
            brpc::Channel* sub_channel = new brpc::Channel;
            std::string server_addr = iter->ip + ":8000";
            if(sub_channel->Init(server_addr.c_str(), &sub_options) != 0){
                LOG(ERROR) << "Fail to initialize sub_channel for " << server_addr;
                return -1;
            }
            if(channel.AddChannel(sub_channel, brpc::OWNS_CHANNEL, mapper, merger) != 0){
                LOG(ERROR) << "Fail to add sub_channel for " << server_addr;
                return -1;
            }
            std::cout << ">> query will be executed on " << iter->ip << std::endl;
            std::cout << std::endl;
        }
    }

    if (channel.channel_count() > 0)
    {
        QueryService_Stub stub(&channel);
        QueryUsersRequest request;
        QueryUsersResponse response;
        brpc::Controller cntl;
        
        stub.QueryUsers(&cntl, &request, &response, NULL);
        if (!cntl.Failed())
        {
            for (int i = 0; i < cntl.sub_count(); i++)
            {
                if(!cntl.sub(i)){
                    LOG(ERROR) << "a controller was lost!";
                    return -1;
                }
                if(cntl.sub(i)->Failed()){
                    LOG(ERROR) << "rpc for channel " << cntl.sub(i)->remote_side().ip << " Failed!";
                    LOG(ERROR) << cntl.sub(i)->ErrorText();
                    return -1;
                }
            }
            
        }
        
        if (op == UNION)
        {
            // just print result
            std::cout << ">> final result : (id, fullname, password)" << std::endl;
            // auto iter_id = response.id().cbegin();
            // auto iter_fullname = response.fullname().cbegin();
            // auto iter_password = response.password().cbegin();
            // while(iter_id != response.id().cend() && iter_fullname != response.fullname().cend() && iter_password != response.password().cend())
            // {
            //     std::cout << "(" << *iter_id << ", " << *iter_fullname << ", " << *iter_password << ")" << std::endl;
            //     iter_id++;
            //     iter_fullname++;
            //     iter_password++;
            // }
            int count = response.id_size();
            for (int i = 0; i < count; i++)
            {
                std::cout << "(" << response.id(i) << ", " << response.fullname(i) << ", " << response.password(i) << ")" << std::endl;
            }
            std::cout << std::endl;
        }
        else{
            // join
        }
        
    }
    


    return 0;
}
