#include <vector>
#include <string>
#include <iostream>

#include "SelectStatement.h"

SelectStatement::SelectStatement() {}

void SelectStatement::add_from(std::string table_name) {
    this->from.push_back(table_name);
}

void SelectStatement::add_select(std::string attribute_name) {
    this->select.push_back(attribute_name);
}

void SelectStatement::add_where(Predicate predicate) {
    this->where.push_back(predicate);
}

void SelectStatement::add_where(int op_type, std::string aname, double num) {
    this->where.push_back(Predicate(op_type, aname, num));
}

void SelectStatement::add_where(int op_type, std::string aname, std::string str) {
    this->where.push_back(Predicate(op_type, aname, str));
}

void SelectStatement::add_where(int op_type, std::vector<std::string> join) {
    this->where.push_back(Predicate(op_type, join));
}

std::ostream& operator<<(std::ostream& os, SelectStatement s) {
    os << "select_list: ";
    for(auto attribute : s.select) {
        os << attribute << "\t";
    }
    os << "\n";
    os << "from_list: ";
    for(auto rname : s.from) {
        os << rname << "\t";
    }
    os << "\n";
    if(s.where.size() > 0) {
        os << "where_list: ";
        for(int i=0; i<s.where.size(); ++i) {
            if(i > 0) os << " AND ";
            os << s.where[i];
        }
        os << "\n";
    }
    return os;
}