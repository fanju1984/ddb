#include "network.h"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

int main(int argc, char* argv[]){

//    int result = write_kv_to_etcd("/test-key1", "test-value1");
//    std::cout << result << std::endl;
//    std::cout << read_from_etcd_by_key("/test-key1");

        // std::string host = "127.0.0.1:8000";
        // std::string table_name = "test";
        // std::string attr_meta = "`id` int(11) not null, `name` varchar(20) not null";
        // std::vector<std::string> attr_values;
        // attr_values.emplace_back("1, 'a'");
        // attr_values.emplace_back("2, 'b'");
        // attr_values.emplace_back("3, 'c'");

        // std::vector<std::string> hosts;
        // hosts.emplace_back("10.77.70.172:8000");
        // hosts.emplace_back("10.77.70.188:8000");
        // hosts.emplace_back("10.77.70.189:8000");
        // for (int i = 0; i < 3; i++)
        // {
        //     load_table(hosts[i], table_name + std::to_string(i), attr_meta, attr_values);
        // }
        
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


    write_map_to_etcd(kv_mp);
    std::set<std::string> temp_tables;
    for(auto iter : kv_mp){
        temp_tables.insert(iter.first.substr(0, iter.first.find('.')));
    }
    std::string root_temp_table = *(temp_tables.cbegin());
    std::cout << "root: " << root_temp_table << std::endl;
    std::vector<std::string> rows = request_table(root_temp_table);
    for(const std::string& row : rows){
        std::cout << row << std::endl;
    }
    std::cout << "total: " << rows.size() << " rows" << std::endl;
    std::vector<std::string> v(temp_tables.cbegin(), temp_tables.cend());
    try
    {
        std::map<std::string, std::string> statistics = get_request_statistics(v);
        // latency and cc
        std::string lm = statistics[root_temp_table];
        std::cout << "root temp table lm: " << lm << std::endl;
        long latency = std::stol(lm.substr(0, lm.find(',')));
        long cc = 0;
        for(auto iter : statistics){
            std::cout << iter.first << ": " << iter.second << std::endl;
            cc += std::stol(iter.second.substr(iter.second.find(',') + 1));
        }
        std::cout << "latency(MS): " << latency / 1000.0 << std::endl;
        std::cout << "communication cost(KB): " << cc / 1000.0 << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    // delete query tree in etcd
    delete_from_etcd_by_prefix(root_temp_table);


    // return load_table(host, table_name, attr_meta, attr_values);

    return 0;
}