#ifndef SQLPROCESSOR_H
#define SQLPROCESSOR_H

#include <vector>
#include <string>

// #include "hsql/SQLParser.h"
// #include "hsql/util/sqlhelper.h"
#include "../../third-party/sql-parser/include/hsql/SQLParser.h"
#include "../../third-party/sql-parser/include/hsql/util/sqlhelper.h"
#include "SelectStatement.h"
#include "InsertStatement.h"
#include "DeleteStatement.h"
#include "../utils/utils.h"
#include "../data-loader/Relation.h"

class SQLProcessor {
private:
    hsql::SelectStatement* select_stat;
    hsql::InsertStatement* insert_stat;
    hsql::DeleteStatement* delete_stat;
    hsql::SQLParserResult result;
    hsql::SQLStatement* stat;
    
    void solve_expr(hsql::Expr* expr, std::vector<Predicate>& where);  // solve the expr recursively
    std::string get_aname_from_expr(hsql::Expr* expr);
    std::vector<std::string> get_anames(std::string rname);
    bool exist_relation(std::string rname);
    bool exist_attribute(std::string rname, std::string aname);
public:
    std::string query;
    SelectStatement select;
    InsertStatement insert;
    DeleteStatement delete_s;
    std::vector<Relation> relations;
    int sql_type;   // 1:select 2:insert 3.delete
    bool is_valid;    // 判断sql中的表名、属性名是否有效

    SQLProcessor(std::string q, std::vector<Relation> relations);
    Relation get_relation_by_rname(std::string rname);
};

#endif