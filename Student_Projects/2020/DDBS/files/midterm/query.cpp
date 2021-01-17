#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/server.h>
#include "query.pb.h"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


DEFINE_int32(port, 8000, "TCP Port of this server");
DEFINE_int32(idle_timeout_s, -1, "Connection will be closed if there is no "
             "read/write operations during the last `idle_timeout_s'");
DEFINE_int32(logoff_ms, 2000, "Maximum duration of server's LOGOFF state "
             "(waiting for client to close connection before server stops)");

class QueryServiceImpl : public QueryService{
    public:
        QueryServiceImpl() {};
        virtual ~QueryServiceImpl() {};
        virtual void QueryUsers(google::protobuf::RpcController* cntl_base, const QueryUsersRequest* request, QueryUsersResponse* response, google::protobuf::Closure* done){
            // RAII : make sure that done->run() will be executed to send response...
            brpc::ClosureGuard done_guard(done);
            brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);
            
            execute_query(request->query(0), response);
            // print response for test
            // int size = response->id_size();
            // auto iter_id = response->id().cbegin();
            // auto iter_fullname = response->fullname().cbegin();
            // auto iter_password = response->password().cbegin();
            //std::cout << "---------------------------" << std::endl;
            // std::cout << ">> send data :" << std::endl;
            // while(iter_id != response->id().cend() && iter_fullname != response->fullname().cend() && iter_password != response->password().cend())
            // {
            //     std::cout << "(" << *iter_id << ", " << *iter_fullname << ", " << *iter_password << ")" << std::endl;
            //     iter_id++;
            //     iter_fullname++;
            //     iter_password++;
            // }
            //done_guard.release();
        }
    private:
        void execute_query(std::string query_str, QueryUsersResponse* response){
            sql::mysql::MySQL_Driver* driver;
            sql::Connection* conn;
            sql::Statement* stat;
            sql::ResultSet* rs;

            driver = sql::mysql::get_driver_instance();
            conn = driver->connect("127.0.0.1", "root", "123456");
            stat = conn->createStatement();
            stat->execute("use test;");
            rs = stat->executeQuery(query_str);
            sql::ResultSetMetaData* rsm = rs->getMetaData();
            int count = rsm->getColumnCount();
            std::vector<std::string> column_names;
            for (int i = 1; i <= count; i++)
            {
                column_names.push_back(rsm->getColumnName(i));
            }
            while (rs->next() != NULL)
            {  
                for (std::string column_name : column_names)
                {
                    add_field(column_name, rs->getString(column_name), response);
                }
            }
            rs->close();
            stat->close();
            conn->close();
        }

        void add_field(std::string field_name, std::string field_value, QueryUsersResponse* response){
            if(field_name == "id"){
                response->add_id(std::stoi(field_value));
            }
            else if(field_name == "name"){
                response->add_name(field_value);
            }
            else if(field_name == "fullname"){
                response->add_fullname(field_value);
            }
            else if(field_name == "password"){
                response->add_password(field_value);
            }
        }


};

int main(int argc, char* argv[]){

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    brpc::Server server;

    QueryServiceImpl query_service;

    if (server.AddService(&query_service, brpc::SERVER_DOESNT_OWN_SERVICE) != 0)
    {
        LOG(ERROR) << "Fail to add QueryService";
        return -1;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = FLAGS_idle_timeout_s;
    if (server.Start(FLAGS_port, &options) != 0) {
        LOG(ERROR) << "Fail to start QueryServer";
        return -1;
    }

    server.RunUntilAskedToQuit();

    return 0;
}
