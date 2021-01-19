#include "DeleteStatement.h"

std::ostream& operator<<(std::ostream& os, DeleteStatement s) {
    os << "delete_rname: " << s.rname << "\n";
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
