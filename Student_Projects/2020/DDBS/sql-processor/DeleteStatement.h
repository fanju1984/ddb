#ifndef DELETESTATEMENT_H
#define DELETESTATEMENT_H

#include <vector>
#include <string>

#include "../data-loader/Predicate.h"

class DeleteStatement {
private:
    friend std::ostream& operator<<(std::ostream& os, DeleteStatement s);
public:
    std::string rname;
    std::vector<Predicate> where;
};

#endif