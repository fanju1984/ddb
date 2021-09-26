#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/channel.h>
#include "base64.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
// #include <mysql_connection.h>
// #include <mysql_driver.h>
// #include <cppconn/driver.h>
// #include <cppconn/exception.h>
// #include <cppconn/resultset.h>
// #include <cppconn/statement.h>
// #include <cppconn/prepared_statement.h>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 2000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_string(protocol, "http", "Client-side protocol");

namespace brpc {
    DECLARE_bool(http_verbose);
}

struct fragment
{
    std::string db_name;
    std::string table_name;
    int fragno;
    std::string ip;
    int max_id;
    int min_id;
    std::string type;
    std::string sql;
};

int write_to_etcd(brpc::Channel* channel, brpc::Controller* cntl, char* put_url, std::string key, std::string value);
int generate_sql_script(int fragnum, std::vector<std::string> ips){
    std::string db_name = "test";
    std::string table_name = "users";
    std::string createSql = R"(use test;
    DROP TABLE IF EXISTS users;
        CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL,
  `name` varchar(20) DEFAULT NULL,
  `fullname` varchar(40) DEFAULT NULL,
  `password` varchar(12) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

)";

    std::string insertSql = "INSERT INTO `users` (`id`, `name`, `fullname`, `password`) VALUES";
    std::unordered_map<int, fragment> mp;
    for(int i = 0; i < fragnum; i++){
        fragment frag;
        frag.fragno = i;
        frag.db_name = db_name;
        frag.table_name = table_name;
        frag.ip = ips[i];
        frag.max_id = -1;
        frag.min_id = 100;
        frag.type = "horizontal";
        frag.sql = createSql;
        mp[i] = frag;
    }


    std::string data = R"(
(1, 'ed', 'Ed Jones', 'edspassword')
(2, 'wendy', 'Wendy Williams', 'foobar')
(3, 'mary', 'Mary Contrary', 'xxg527')
(4, 'lisa', 'lisa Contrary', 'ls123')
(5, 'cred', 'cred Flinstone', 'bla123')
(6, 'fred', 'Fred Flinstone', 'blah')
(7, 'jack', 'Jack Bean', 'gjffdd')
(8, 'ed', 'Ed Jones', 'edspassword')
(9, 'jack', 'Jack Bean', 'gjffdd')
(10, 'ed', 'Ed Jones', '888')
(11, 'wendy', 'Wendy Williams', 'foobar')
(12, 'mary', 'Mary Contrary', '123')
(13, 'lisa', 'lisa Contrary', 'ls123')
(14, 'cred', 'cred Flinstone', 'bla123')
(15, 'fred', 'Fred Flinstone', 'blah')
(16, 'jack', 'Jack Bean', 'gjffdd')
(17, 'wxw01', 'Jack wxw', '123')
(18, 'wxw02', 'Jack wxw2', '234'))";


    std::stringstream ss(data);
    std::string line;
    int lineno = 0;
    LOG(INFO) << "insert data...";
    while (getline(ss, line))
    {
        if(line.empty()){
            continue;
        }
        std::cout << line << std::endl;
        int fragno = lineno % fragnum;
        if (mp[fragno].max_id < lineno + 1)
        {
            mp[fragno].max_id = lineno + 1;
        }
        if(mp[fragno].min_id > lineno + 1){
            mp[fragno].min_id = lineno + 1;
        }
        lineno++;
        mp[fragno].sql += (insertSql + line + ";\n");
    }
    std::ofstream out;

    char* put_url = "http://127.0.0.1:2379/v3/kv/put";
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;


    if (channel.Init("http://127.0.0.1:2379", FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;
    // cntl.http_request().uri() = put_url;
    // cntl.http_request().set_method(brpc::HTTP_METHOD_POST);


    if(write_to_etcd(&channel, &cntl, put_url, "/" + db_name + "/" + table_name + "/fragnum", std::to_string(fragnum)) == -1){
        return -1;
    }

    for (auto iter = mp.cbegin(); iter != mp.cend(); iter++)
    {
        // 1.write sql script
        out.open(R"(../sql-scripts/)" + iter->second.db_name + "-"+ iter->second.table_name + "-" + std::to_string(iter->first) + "-" + iter->second.ip+".sql", std::ofstream::out | std::ofstream::trunc);
        if (out.is_open())
        {
            LOG(INFO) << "write file...";
            std::cout << iter->second.sql;
            out << iter->second.sql;
            out.flush();
            out.close();
        }



        // 2.write to etcd
        // 2.1 ip
        if(write_to_etcd(&channel, &cntl, put_url, "/" + iter->second.db_name + "/" + iter->second.table_name + "/" + std::to_string(iter->second.fragno) + "/ip", iter->second.ip) == -1){
            return -1;
        }
        if(write_to_etcd(&channel, &cntl, put_url, "/" + iter->second.db_name + "/" + iter->second.table_name + "/" + std::to_string(iter->second.fragno) + "/type", iter->second.type) == -1){
            return -1;
        }
        if(write_to_etcd(&channel, &cntl, put_url, "/" + iter->second.db_name + "/" + iter->second.table_name + "/" + std::to_string(iter->second.fragno) + "/max_id", std::to_string(iter->second.max_id)) == -1){
            return -1;
        }
        if(write_to_etcd(&channel, &cntl, put_url, "/" + iter->second.db_name + "/" + iter->second.table_name + "/" + std::to_string(iter->second.fragno) + "/min_id", std::to_string(iter->second.min_id)) == -1){
            return -1;
        }
    }
    return 0;
}

int write_to_etcd(brpc::Channel* channel, brpc::Controller* cntl, char* put_url, std::string key, std::string value){
    LOG(INFO) << "put <" << key << "," << value << ">";
    nlohmann::json j;
    j["key"] = base64_encode(key);
    j["value"] = base64_encode(value);
    cntl->http_request().uri() = put_url;
    cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
    cntl->request_attachment().append(j.dump());
    channel->CallMethod(NULL, cntl, NULL, NULL, NULL);
    if(cntl->Failed()){
        LOG(ERROR) << cntl->ErrorText() << std::endl;
        return -1;
    }
    j.clear();
    cntl->Reset();
    return 0;
}

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

int main(int argc, char* argv[]){
     std::vector<std::string> ips;
     ips.push_back("10.77.70.172");
     ips.push_back("10.77.70.189");
     ips.push_back("10.77.70.188");
     return generate_sql_script(3, ips);

//    GFLAGS_NS::ParseCommandLineFlags(&argc, &argv, true);
//
//    char* put_url = "http://127.0.0.1:2379/v3/kv/put";
//    char* get_url = "http://127.0.0.1:2379/v3/kv/range";
//
//    brpc::Channel channel;
//    brpc::ChannelOptions options;
//    options.protocol = FLAGS_protocol;
//    options.timeout_ms = FLAGS_timeout_ms;
//    options.max_retry = FLAGS_max_retry;
//    if (channel.Init("http://127.0.0.1:2379", FLAGS_load_balancer.c_str(), &options) != 0){
//        LOG(ERROR) << "Fail to initialize channel";
//        return -1;
//    }
//
//    brpc::Controller cntl;

    // // 1.set kv
    // cntl.http_request().uri() = put_url;
    // cntl.http_request().set_method(brpc::HTTP_METHOD_POST);

    // nlohmann::json j;
    // j["key"] = base64_encode("/my-test-key");
    // j["value"] = base64_encode("my-test-value");
    // cntl.request_attachment().append(j.dump());

    // channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
    // if(cntl.Failed()){
    //     LOG(ERROR) << cntl.ErrorText() << std::endl;
    //     return -1;
    // }
    // LOG(INFO) << "set </my-test-key:my-test-value.>";

    // 2.get kv
    // cntl.Reset();
//    cntl.http_request().uri() = get_url;
//    cntl.http_request().set_method(brpc::HTTP_METHOD_POST);
//
//    // j.clear();
//    j["key"] = base64_encode("/test/users");
//    j["range_end"] = base64_encode("/test/usert");
//    cntl.request_attachment().append(j.dump());
//
//    channel.CallMethod(NULL, &cntl, NULL, NULL, NULL);
//    if(cntl.Failed()){
//        LOG(ERROR) << cntl.ErrorText();
//        return -1;
//    }
//
//    LOG(INFO) << cntl.response_attachment();
    return 0;
}
