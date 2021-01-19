#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>

// etcd
int write_kv_to_etcd(const std::string& key, const std::string& value);
int write_map_to_etcd(const std::map<std::string, std::string>& mp);
std::string read_from_etcd_by_key(const std::string& key);
std::unordered_map<std::string, std::string> read_from_etcd_by_prefix(const std::string& prefix);
int delete_from_etcd_by_key(const std::string& key);
int delete_from_etcd_by_prefix(const std::string& prefix);
std::map<std::string, std::string> read_map_from_etcd(const std::vector<std::string>& keys);

// rpc
int load_table(const std::string& host, const std::string& table_name, const std::string& attr_meta, const std::vector<std::string>& attr_values);
std::vector<std::string> request_table(const std::string& temp_table_name);
std::map<std::string, std::string> get_request_statistics(const std::vector<std::string>& temp_table_names);
std::string execute_non_query_sql(const std::string& ip, const std::string& sql);
std::vector<std::string> execute_query_sql(const std::string& ip, const std::string& sql);