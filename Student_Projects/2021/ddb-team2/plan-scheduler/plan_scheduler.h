/* plan_scheduler.h

    Query-plan scheduling.

    Created by khaiwang

    2021/11/16
*/

#include "gflags/gflags.h"
#include "brpc/channel.h"
#include "brpc/parallel_channel.h"
#include "brpc/server.h"
#include <string>

#include "ddb.pb.h"
#include "json.hpp"
#include "base64.h"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

class SQLServiceImpl : public sql_service {
public:
    void RequestNode(::google::protobuf::RpcController *controller, const ::NodeRequest *request,
                      ::NodeResponse *response, ::google::protobuf::Closure *done) override;

    void ImportData(::google::protobuf::RpcController *controller, const ::ImportDataRequest *request,
                   ::ImportDataResponse *response, ::google::protobuf::Closure *done) override;

    void DeleteTable(::google::protobuf::RpcController *controller, const ::DeleteTempTableRequest *request,
                     ::DeleteTempTableResponse *response, ::google::protobuf::Closure *done) override;

    void ExecuteNonQuerySQL(::google::protobuf::RpcController *controller, const ::ExecuteNonQuerySQLRequest *request,
                            ::ExecuteNonQuerySQLResponse *response, ::google::protobuf::Closure *done) override;

    void ExecuteQuerySQL(::google::protobuf::RpcController* controller,
                       const ::ExecuteQuerySQLRequest* request,
                       ::ExecuteQuerySQLResponse* response,
                       ::google::protobuf::Closure* done) override;                        
};
