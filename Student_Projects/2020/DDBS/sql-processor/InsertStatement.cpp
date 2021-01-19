#include "InsertStatement.h"

InsertStatement::InsertStatement() {
    this->rname = "";
    this->values = "";
}

void InsertStatement::add_value(double value) {
    if(this->values != "") {
        this->values += ", ";
    }
    this->values += double2string(value);
}

void InsertStatement::add_value(int value) {
    if(this->values != "") {
        this->values += ", ";
    }
    this->values += std::to_string(value);
}

void InsertStatement::add_value(std::string value) {
    if(this->values != "") {
        this->values += ", ";
    }
    this->values = this->values + "'" + value + "'";
}

std::ostream& operator<<(std::ostream& os, InsertStatement s) {
    os << "insert_rname: " << s.rname  << "\n";
    os << "insert_values: " << s.values << "\n";
    return os;
}
