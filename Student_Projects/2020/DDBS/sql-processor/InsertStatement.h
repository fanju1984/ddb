#ifndef INSERTSTATEMENT_H
#define INSERTSTATEMENT_H

#include <vector>
#include <string>

#include "../data-loader/Predicate.h"
#include "../utils/utils.h"

class InsertStatement {
private:
    friend std::ostream& operator<<(std::ostream& os, InsertStatement s);
public:
    std::string rname;
    std::string values;

    InsertStatement();
    void add_value(double value);
    void add_value(int value);
    void add_value(std::string value);
};

#endif