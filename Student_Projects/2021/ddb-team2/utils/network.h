/* network.h

    RPCs for query-plan scheduling.
    
    Created by khaiwang

    2021/11/16
*/

#ifndef NETWORK_H
#define NETWORK_H
#include "ddb.pb.h"

#include <string>
#include <map>
#include "json.hpp"
#include "base64.h"
#include "brpc/channel.h"


int write_kv_to_etcd(const std::string &key, const std::string &value);
int write_map_to_etcd(const std::map<std::string, std::string> &map);
std::string read_kv_from_etcd(const std::string &key);
int delete_kv_from_etcd(const std::string &key);
int write_to_etcd(const std::string &key, const nlohmann::json &value);
int delete_structure_from_etcd(const std::string &key);
nlohmann::json read_from_etcd(const std::string &key);

// int import_data(const std::string& host, const std::string& table_name, 
//                 const std::string& table_fields, const std::vector<std::string>& table_values);
// std::vector<std::string> request_node(const std::string& temp_table_name);
// std::string execute_non_query_sql(const std::string& ip, const std::string& sql);
std::vector<std::string> execute_query_sql(const std::string& ip, const std::string& sql);

#endif