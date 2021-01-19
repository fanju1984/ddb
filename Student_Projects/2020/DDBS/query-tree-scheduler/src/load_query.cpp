#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <typeinfo>

#include "gflags/gflags.h"
#include "brpc/channel.h"


#include "json.hpp"
#include "base64.h"


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


DEFINE_string(load_balancer, "", "The algorithm for load balancing");
DEFINE_int32(timeout_ms, 30000, "RPC timeout in milliseconds");
DEFINE_int32(max_retry, 3, "Max retries(not including the first RPC)");
DEFINE_string(protocol, "http", "Client-side protocol");

// node type
const std::string LEAF = "L";
const std::string NON_LEAF = "NL";

// combine op
const std::string UNION = "U";
const std::string JOIN = "J";

// seperator
const std::string OUTER_SEPERATOR = "|";
const std::string INNER_SEPERATOR = ":";

const std::string ETCD_PUT_URL = "http://127.0.0.1:2379/v3/kv/put";

const std::string TEMP_TABLE_PREFIX =  "temp_table";


int auto_increment_id = 0;

void check_host_name(int hostname) { //This function returns host name for local computer
    if (hostname == -1) {
        perror("gethostname");
        exit(1);
    }
}
void check_host_entry(struct hostent * hostentry) { //find host info from host name
    if (hostentry == NULL){
        perror("gethostbyname");
        exit(1);
    }
}
void IP_formatter(char *IPbuffer) { //convert IP string to dotted decimal format
    if (NULL == IPbuffer) {
        perror("inet_ntoa");
        exit(1);
    }
}

int get_site_no(){
    char host[256];
    char *IP;
    struct hostent *host_entry;
    int hostname;
    hostname = gethostname(host, sizeof(host)); //find the host name
    check_host_name(hostname);
    host_entry = gethostbyname(host); //find host information
    check_host_entry(host_entry);
    IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
//    printf("Current Host Name: %s\n", host);
//    printf("Host IP: %s\n", IP);
    std::string ip = IP;
    if (ip == "10.77.70.72"){
        return 1;
    } else if (ip == "127.0.0.1"){
        return 2;
    } else if (ip == "127.0.0.1"){
        return 3;
    }
    return 0;
}


int load_temp_table(brpc::Channel* channel, brpc::Controller* cntl, std::map<std::string, std::string> mp){
    for (auto iter = mp.cbegin(); iter != mp.cend(); iter++) {
        std::string key = iter->first;
        std::string value = iter->second;
        LOG(INFO) << "put <" << key << "," << value << ">";
        nlohmann::json j;
        j["key"] = base64_encode(key);
        j["value"] = base64_encode(value);
        cntl->http_request().uri() = ETCD_PUT_URL;
        cntl->http_request().set_method(brpc::HTTP_METHOD_POST);
        cntl->request_attachment().append(j.dump());
        channel->CallMethod(NULL, cntl, NULL, NULL, NULL);
        if(cntl->Failed()){
            LOG(ERROR) << cntl->ErrorText() << std::endl;
            return -1;
        }
        cntl->Reset();
    }
    return 0;
}
int load_query1_for_test(){

    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;
    std::unordered_map<int, std::string> site_ip_mp;
    site_ip_mp[1] = "127.0.0.1";
    site_ip_mp[2] = "127.0.0.1";
    site_ip_mp[3] = "127.0.0.1";
    site_ip_mp[4] = "127.0.0.1";
    site_ip_mp[5] = "127.0.0.1";

    std::vector<std::string> temp;
    int unique_query_id = 1;
    std::string root_table_name = "query" + std::to_string(unique_query_id) + "_" + TEMP_TABLE_PREFIX;
//    std::string prefix = "/" + root_table_name;

    kv_mp[root_table_name + ".type"] = NON_LEAF;
    kv_mp[root_table_name + ".combine"] = root_table_name + "_1.customer_id = " + root_table_name + "_2.customer_id";
    kv_mp[root_table_name + ".children"] = root_table_name + "_1" + INNER_SEPERATOR + site_ip_mp[5] + OUTER_SEPERATOR + root_table_name + "_2" + INNER_SEPERATOR + site_ip_mp[5];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    kv_mp[root_table_name + "_1.type"] = LEAF;
    kv_mp[root_table_name + "_1.children"] = "site1_customer";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    kv_mp[root_table_name + "_2.type"] = LEAF;
    kv_mp[root_table_name + "_2.children"] = "site2_customer";
    load_temp_table(&channel, &cntl, kv_mp);
    kv_mp.clear();
    return 0;
}


int load_query2_for_test(){

    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;
    std::unordered_map<int, std::string> site_ip_mp;
    site_ip_mp[1] = "127.0.0.1";
    site_ip_mp[2] = "127.0.0.1";
    site_ip_mp[3] = "127.0.0.1";
    site_ip_mp[4] = "127.0.0.1";
    site_ip_mp[5] = "127.0.0.1";

    std::vector<std::string> temp;
    int unique_query_id = 2;
    std::string root_table_name = "query" + std::to_string(unique_query_id) + "_" + TEMP_TABLE_PREFIX;
//    std::string prefix = "/" + root_table_name;
    kv_mp[root_table_name + ".type"] = NON_LEAF;
    kv_mp[root_table_name + ".combine"] = UNION;
    kv_mp[root_table_name + ".children"] = root_table_name + "_1" + INNER_SEPERATOR + site_ip_mp[5] + OUTER_SEPERATOR + root_table_name + "_2" + INNER_SEPERATOR + site_ip_mp[5] + OUTER_SEPERATOR + root_table_name + "_3" + INNER_SEPERATOR + site_ip_mp[5];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    kv_mp[root_table_name + "_1.type"] = LEAF;
    kv_mp[root_table_name + "_1.project"] = "title";
    kv_mp[root_table_name + "_1.select"] = "copies > 5000";
    kv_mp[root_table_name + "_1.children"] = "site1_book";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    kv_mp[root_table_name + "_2.type"] = LEAF;
    kv_mp[root_table_name + "_2.project"] = "title";
    kv_mp[root_table_name + "_2.select"] = "copies > 5000";
    kv_mp[root_table_name + "_2.children"] = "site2_book";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    kv_mp[root_table_name + "_3.type"] = LEAF;
    kv_mp[root_table_name + "_3.project"] = "title";
    kv_mp[root_table_name + "_3.select"] = "copies > 5000";
    kv_mp[root_table_name + "_3.children"] = "site3_book";
    load_temp_table(&channel, &cntl, kv_mp);
    kv_mp.clear();
    return 0;
}

// load query 9
int load_query9_for_test(){

    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;
    std::map<int, std::string> site_ip_mp;
    site_ip_mp[1] = "127.0.0.1";//"127.0.0.1";
    site_ip_mp[2] = "127.0.0.1";//"127.0.0.1";
    site_ip_mp[3] = "127.0.0.1";//"127.0.0.1";
    site_ip_mp[4] = "127.0.0.1";//"127.0.0.1";
    site_ip_mp[5] = "127.0.0.1";
    std::vector<std::string> temp;
    int unique_query_id = 9;
    std::string prefix = "query_" + std::to_string(get_site_no()) + "_" + std::to_string(auto_increment_id) + "_" + TEMP_TABLE_PREFIX;

    // temp_table
    kv_mp[prefix + ".type"] = NON_LEAF;
    kv_mp[prefix + ".combine"] = UNION;
    kv_mp[prefix + ".children"] = prefix + "_1" + INNER_SEPERATOR + site_ip_mp[2] + OUTER_SEPERATOR + prefix + "_2" + INNER_SEPERATOR + site_ip_mp[4];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1
    kv_mp[prefix + "_1" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_1" + ".project"] = "customer_name, book_title, publisher_name, orders_quantity";
    kv_mp[prefix + "_1" + ".combine"] = prefix + "_1_1." + "book_id = " + prefix + "_1_2." + "orders_book_id";
    kv_mp[prefix + "_1" + ".children"] = prefix + "_1_1" + INNER_SEPERATOR + site_ip_mp[2] + OUTER_SEPERATOR + prefix + "_1_2" + INNER_SEPERATOR + site_ip_mp[2];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_2
    kv_mp[prefix + "_2" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_2" + ".project"] = "customer_name, book_title, publisher_name, orders_quantity";
    kv_mp[prefix + "_2" + ".combine"] = prefix + "_2_1." + "orders_book_id = " + prefix + "_2_2." + "book_id";
    kv_mp[prefix + "_2" + ".children"] = prefix + "_2_1" + INNER_SEPERATOR + site_ip_mp[4] + OUTER_SEPERATOR + prefix + "_2_2" + INNER_SEPERATOR + site_ip_mp[4];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1_1
    kv_mp[prefix + "_1_1" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_1_1" + ".combine"] = prefix + "_1_1_1." + "book_publisher_id = " + prefix + "_1_1_2." + "publisher_id";
    kv_mp[prefix + "_1_1" + ".children"] = prefix + "_1_1_1" + INNER_SEPERATOR + site_ip_mp[3] + OUTER_SEPERATOR + prefix + "_1_1_2" + INNER_SEPERATOR + site_ip_mp[2];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1_2
    kv_mp[prefix + "_1_2" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_1_2" + ".combine"] = prefix + "_1_2_1." + "orders_customer_id = " + prefix + "_1_2_2." + "customer_id";
    kv_mp[prefix + "_1_2" + ".children"] = prefix + "_1_2_1" + INNER_SEPERATOR + site_ip_mp[2] + OUTER_SEPERATOR + prefix + "_1_2_2" + INNER_SEPERATOR + site_ip_mp[1];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.1
    kv_mp[prefix + "_2_1" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_2_1" + ".combine"] = prefix + "_2_1_1." + "orders_customer_id = " + prefix + "_2_1_2." + "customer_id";
    kv_mp[prefix + "_2_1" + ".children"] = prefix + "_2_1_1" + INNER_SEPERATOR + site_ip_mp[4] + OUTER_SEPERATOR + prefix + "_2_1_2" + INNER_SEPERATOR + site_ip_mp[1];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.2
    kv_mp[prefix + "_2_2" + ".type"] = NON_LEAF;
    kv_mp[prefix + "_2_2" + ".combine"] = prefix + "_2_2_1." + "publisher_id = " + prefix + "_2_2_2." + "book_publisher_id";
    kv_mp[prefix + "_2_2" + ".children"] = prefix + "_2_2_1" + INNER_SEPERATOR + site_ip_mp[4] + OUTER_SEPERATOR + prefix + "_2_2_2" + INNER_SEPERATOR + site_ip_mp[3];
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.1.1.1
    kv_mp[prefix + "_1_1_1" + ".type"] = LEAF;
    kv_mp[prefix + "_1_1_1" + ".project"] = "id, title, publisher_id";
    kv_mp[prefix + "_1_1_1" + ".select"] = "id > 220000";
    kv_mp[prefix + "_1_1_1" + ".children"] = "site3_book";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.1.1.2
    kv_mp[prefix + "_1_1_2" + ".type"] = LEAF;
    kv_mp[prefix + "_1_1_2" + ".project"] = "id, name";
    kv_mp[prefix + "_1_1_2" + ".children"] = "site2_publisher";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.1.2.1
    kv_mp[prefix + "_1_2_1" + ".type"] = LEAF;
    kv_mp[prefix + "_1_2_1" + ".select"] = "quantity > 1";
    kv_mp[prefix + "_1_2_1" + ".children"] = "site2_orders";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.1.2.2
    kv_mp[prefix + "_1_2_2" + ".type"] = LEAF;
    kv_mp[prefix + "_1_2_2" + ".children"] = "site1_customer";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.1.1
    kv_mp[prefix + "_2_1_1" + ".type"] = LEAF;
    kv_mp[prefix + "_2_1_1" + ".select"] = "quantity > 1";
    kv_mp[prefix + "_2_1_1" + ".children"] = "site4_orders";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.1.2
    kv_mp[prefix + "_2_1_2" + ".type"] = LEAF;
    kv_mp[prefix + "_2_1_2" + ".children"] = "site1_customer";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.2.1
    kv_mp[prefix + "_2_2_1" + ".type"] = LEAF;
    kv_mp[prefix + "_2_2_1" + ".project"] = "id, name";
    kv_mp[prefix + "_2_2_1" + ".children"] = "site4_publisher";
//    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    // temp_table_1.2.2.2
    kv_mp[prefix + "_2_2_2" + ".type"] = LEAF;
    kv_mp[prefix + "_2_2_2" + ".project"] = "id, title, publisher_id";
    kv_mp[prefix + "_2_2_2" + ".select"] = "id > 220000";
    kv_mp[prefix + "_2_2_2" + ".children"] = "site3_book";
    load_temp_table(&channel, &cntl, kv_mp);
//    kv_mp.clear();

    for(const auto& iter : kv_mp){
        std::cout << iter.first << " --> " << iter.second << std::endl;
    }
    return 0;
}


int load_query9_1_for_test(){
    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;

    kv_mp["query_0_0_temp_table.children"] = "query_0_0_temp_table_1:127.0.0.1|query_0_0_temp_table_2:127.0.0.1";
    kv_mp["query_0_0_temp_table.combine"] = "query_0_0_temp_table_1.publisher_id = query_0_0_temp_table_2.book_publisher_id";
    kv_mp["query_0_0_temp_table.project"] = "publisher_name,book_title,customer_name,orders_quantity";
    kv_mp["query_0_0_temp_table.type"] = "NL";
    kv_mp["query_0_0_temp_table_1.children"] = "query_0_0_temp_table_1_1:127.0.0.1|query_0_0_temp_table_1_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1.combine"] = "U";
    kv_mp["query_0_0_temp_table_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1.children"] = "site2_publisher";
    kv_mp["query_0_0_temp_table_1_1.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_2.children"] = "site4_publisher";
    kv_mp["query_0_0_temp_table_1_2.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_2.type"] = "L";
    kv_mp["query_0_0_temp_table_2.children"] = "query_0_0_temp_table_2_1:127.0.0.1|query_0_0_temp_table_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2.combine"] = "query_0_0_temp_table_2_1.book_id = query_0_0_temp_table_2_2.orders_book_id";
    kv_mp["query_0_0_temp_table_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1.children"] = "site3_book";
    kv_mp["query_0_0_temp_table_2_1.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_2_1.select"] = "id > 220000";
    kv_mp["query_0_0_temp_table_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2.children"] = "query_0_0_temp_table_2_2_1:127.0.0.1|query_0_0_temp_table_2_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_2.combine"] = "query_0_0_temp_table_2_2_1.customer_id = query_0_0_temp_table_2_2_2.orders_customer_id";
    kv_mp["query_0_0_temp_table_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_2_1.children"] = "site1_customer";
    kv_mp["query_0_0_temp_table_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2_2.children"] = "query_0_0_temp_table_2_2_2_1:127.0.0.1|query_0_0_temp_table_2_2_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_2_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_2_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_2_2_1.children"] = "site2_orders";
    kv_mp["query_0_0_temp_table_2_2_2_1.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2_2_2.children"] = "site4_orders";
    kv_mp["query_0_0_temp_table_2_2_2_2.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_2_2_2.type"] = "L";
    load_temp_table(&channel, &cntl, kv_mp);
    return 0;
}

int load_query10_for_test(){
    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;

    kv_mp["query_0_0_temp_table.children"] = "query_0_0_temp_table_1:127.0.0.1|query_0_0_temp_table_2:127.0.0.1";
    kv_mp["query_0_0_temp_table.combine"] = "U";
    kv_mp["query_0_0_temp_table.type"] = "NL";
    kv_mp["query_0_0_temp_table_1.children"] = "query_0_0_temp_table_1_1:127.0.0.1|query_0_0_temp_table_1_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1.combine"] = "query_0_0_temp_table_1_1.book_publisher_id = query_0_0_temp_table_1_2.publisher_id";
    kv_mp["query_0_0_temp_table_1.project"] = "book_title,customer_name,orders_quantity,publisher_name";
    kv_mp["query_0_0_temp_table_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1.children"] = "query_0_0_temp_table_1_1_1:127.0.0.1|query_0_0_temp_table_1_1_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1_1.combine"] = "query_0_0_temp_table_1_1_1.book_id = query_0_0_temp_table_1_1_2.orders_book_id";
    kv_mp["query_0_0_temp_table_1_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_1.children"] = "query_0_0_temp_table_1_1_1_1:127.0.0.1|query_0_0_temp_table_1_1_1_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1_1_1.combine"] = "U";
    kv_mp["query_0_0_temp_table_1_1_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_1_1.children"] = "site2_book";
    kv_mp["query_0_0_temp_table_1_1_1_1.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_1_1_1_1.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_1_1_1_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_1_2.children"] = "site1_book";
    kv_mp["query_0_0_temp_table_1_1_1_2.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_1_1_1_2.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_1_1_1_2.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_2.children"] = "query_0_0_temp_table_1_1_2_1:127.0.0.1|query_0_0_temp_table_1_1_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1_1_2.combine"] = "query_0_0_temp_table_1_1_2_1.customer_id = query_0_0_temp_table_1_1_2_2.orders_customer_id";
    kv_mp["query_0_0_temp_table_1_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_2_1.children"] = "site1_customer";
    kv_mp["query_0_0_temp_table_1_1_2_1.select"] = "id > 308000";
    kv_mp["query_0_0_temp_table_1_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_2_2.children"] = "site3_orders";
    kv_mp["query_0_0_temp_table_1_1_2_2.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_1_1_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_1_2.children"] = "query_0_0_temp_table_1_2_1:127.0.0.1|query_0_0_temp_table_1_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_1_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_2_1.children"] = "site1_publisher";
    kv_mp["query_0_0_temp_table_1_2_1.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_2_2.children"] = "site3_publisher";
    kv_mp["query_0_0_temp_table_1_2_2.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_2.children"] = "query_0_0_temp_table_2_1:127.0.0.1|query_0_0_temp_table_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2.combine"] = "query_0_0_temp_table_2_1.book_publisher_id = query_0_0_temp_table_2_2.publisher_id";
    kv_mp["query_0_0_temp_table_2.project"] = "book_title,customer_name,orders_quantity,publisher_name";
    kv_mp["query_0_0_temp_table_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1.children"] = "query_0_0_temp_table_2_1_1:127.0.0.1|query_0_0_temp_table_2_1_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_1.combine"] = "query_0_0_temp_table_2_1_1.book_id = query_0_0_temp_table_2_1_2.orders_book_id";
    kv_mp["query_0_0_temp_table_2_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_1.children"] = "site3_book";
    kv_mp["query_0_0_temp_table_2_1_1.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_2_1_1.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_2_1_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2.children"] = "query_0_0_temp_table_2_1_2_1:127.0.0.1|query_0_0_temp_table_2_1_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_1_2.combine"] = "query_0_0_temp_table_2_1_2_1.customer_id = query_0_0_temp_table_2_1_2_2.orders_customer_id";
    kv_mp["query_0_0_temp_table_2_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_2_1.children"] = "site1_customer";
    kv_mp["query_0_0_temp_table_2_1_2_1.select"] = "id > 308000";
    kv_mp["query_0_0_temp_table_2_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2_2.children"] = "query_0_0_temp_table_2_1_2_2_1:127.0.0.1|query_0_0_temp_table_2_1_2_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_1_2_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_2_1_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.children"] = "site3_orders";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.children"] = "site4_orders";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2.children"] = "query_0_0_temp_table_2_2_1:127.0.0.1|query_0_0_temp_table_2_2_2:127.0.0.1";
    kv_mp["query_0_0_temp_table_2_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_2_1.children"] = "site1_publisher";
    kv_mp["query_0_0_temp_table_2_2_1.project"] = "id,name";
    kv_mp["query_0_0_temp_table_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2_2.children"] = "site3_publisher";
    kv_mp["query_0_0_temp_table_2_2_2.project"] = "id,name";
    kv_mp["query_0_0_temp_table_2_2_2.type"] = "L";
    load_temp_table(&channel, &cntl, kv_mp);
    return 0;
}


int load_query10_server_for_test(){

    // 1.prepare channel
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol = FLAGS_protocol;
    options.timeout_ms = FLAGS_timeout_ms;
    options.max_retry = FLAGS_max_retry;

    if (channel.Init(ETCD_PUT_URL.c_str(), FLAGS_load_balancer.c_str(), &options) != 0){
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    brpc::Controller cntl;

    // 2.write query tree info to etcd
    std::map<std::string, std::string> kv_mp;

    kv_mp["query_0_0_temp_table.children"] = "query_0_0_temp_table_1:10.77.70.172|query_0_0_temp_table_2:10.77.70.189";
    kv_mp["query_0_0_temp_table.combine"] = "U";
    kv_mp["query_0_0_temp_table.type"] = "NL";
    kv_mp["query_0_0_temp_table_1.children"] = "query_0_0_temp_table_1_1:10.77.70.172|query_0_0_temp_table_1_2:10.77.70.172";
    kv_mp["query_0_0_temp_table_1.combine"] = "query_0_0_temp_table_1_1.book_publisher_id = query_0_0_temp_table_1_2.publisher_id";
    kv_mp["query_0_0_temp_table_1.project"] = "book_title,customer_name,orders_quantity,publisher_name";
    kv_mp["query_0_0_temp_table_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1.children"] = "query_0_0_temp_table_1_1_1:10.77.70.172|query_0_0_temp_table_1_1_2:10.77.70.172";
    kv_mp["query_0_0_temp_table_1_1.combine"] = "query_0_0_temp_table_1_1_1.book_id = query_0_0_temp_table_1_1_2.orders_book_id";
    kv_mp["query_0_0_temp_table_1_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_1.children"] = "query_0_0_temp_table_1_1_1_1:10.77.70.188|query_0_0_temp_table_1_1_1_2:10.77.70.172";
    kv_mp["query_0_0_temp_table_1_1_1.combine"] = "U";
    kv_mp["query_0_0_temp_table_1_1_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_1_1.children"] = "site2_book";
    kv_mp["query_0_0_temp_table_1_1_1_1.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_1_1_1_1.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_1_1_1_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_1_2.children"] = "site1_book";
    kv_mp["query_0_0_temp_table_1_1_1_2.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_1_1_1_2.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_1_1_1_2.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_2.children"] = "query_0_0_temp_table_1_1_2_1:10.77.70.172|query_0_0_temp_table_1_1_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_1_1_2.combine"] = "query_0_0_temp_table_1_1_2_1.customer_id = query_0_0_temp_table_1_1_2_2.orders_customer_id";
    kv_mp["query_0_0_temp_table_1_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_1_2_1.children"] = "site1_customer";
    kv_mp["query_0_0_temp_table_1_1_2_1.select"] = "id > 308000";
    kv_mp["query_0_0_temp_table_1_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_1_2_2.children"] = "site3_orders";
    kv_mp["query_0_0_temp_table_1_1_2_2.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_1_1_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_1_2.children"] = "query_0_0_temp_table_1_2_1:10.77.70.172|query_0_0_temp_table_1_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_1_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_1_2_1.children"] = "site1_publisher";
    kv_mp["query_0_0_temp_table_1_2_1.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_1_2_2.children"] = "site3_publisher";
    kv_mp["query_0_0_temp_table_1_2_2.project"] = "id,name";
    kv_mp["query_0_0_temp_table_1_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_2.children"] = "query_0_0_temp_table_2_1:10.77.70.189|query_0_0_temp_table_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_2.combine"] = "query_0_0_temp_table_2_1.book_publisher_id = query_0_0_temp_table_2_2.publisher_id";
    kv_mp["query_0_0_temp_table_2.project"] = "book_title,customer_name,orders_quantity,publisher_name";
    kv_mp["query_0_0_temp_table_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1.children"] = "query_0_0_temp_table_2_1_1:10.77.70.189|query_0_0_temp_table_2_1_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_2_1.combine"] = "query_0_0_temp_table_2_1_1.book_id = query_0_0_temp_table_2_1_2.orders_book_id";
    kv_mp["query_0_0_temp_table_2_1.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_1.children"] = "site3_book";
    kv_mp["query_0_0_temp_table_2_1_1.project"] = "id,publisher_id,title";
    kv_mp["query_0_0_temp_table_2_1_1.select"] = "copies > 100";
    kv_mp["query_0_0_temp_table_2_1_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2.children"] = "query_0_0_temp_table_2_1_2_1:10.77.70.172|query_0_0_temp_table_2_1_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_2_1_2.combine"] = "query_0_0_temp_table_2_1_2_1.customer_id = query_0_0_temp_table_2_1_2_2.orders_customer_id";
    kv_mp["query_0_0_temp_table_2_1_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_2_1.children"] = "site1_customer";
    kv_mp["query_0_0_temp_table_2_1_2_1.select"] = "id > 308000";
    kv_mp["query_0_0_temp_table_2_1_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2_2.children"] = "query_0_0_temp_table_2_1_2_2_1:10.77.70.189|query_0_0_temp_table_2_1_2_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_2_1_2_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_2_1_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.children"] = "site3_orders";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_1_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.children"] = "site4_orders";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.select"] = "quantity > 1";
    kv_mp["query_0_0_temp_table_2_1_2_2_2.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2.children"] = "query_0_0_temp_table_2_2_1:10.77.70.172|query_0_0_temp_table_2_2_2:10.77.70.189";
    kv_mp["query_0_0_temp_table_2_2.combine"] = "U";
    kv_mp["query_0_0_temp_table_2_2.type"] = "NL";
    kv_mp["query_0_0_temp_table_2_2_1.children"] = "site1_publisher";
    kv_mp["query_0_0_temp_table_2_2_1.project"] = "id,name";
    kv_mp["query_0_0_temp_table_2_2_1.type"] = "L";
    kv_mp["query_0_0_temp_table_2_2_2.children"] = "site3_publisher";
    kv_mp["query_0_0_temp_table_2_2_2.project"] = "id,name";
    kv_mp["query_0_0_temp_table_2_2_2.type"] = "L";
    load_temp_table(&channel, &cntl, kv_mp);
    return 0;
}
int main(int argc, char* argv[]){

//    load_query1_for_test();
//    load_query2_for_test();
    // load_query9_1_for_test();
    load_query10_for_test();
    return 0;
}