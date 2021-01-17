#ifndef PREDICATE_H
#define PREDICATE_H

#include <vector>
#include <string>

#include "../utils/utils.h"

class Predicate {
private:
    friend std::ostream& operator<<(std::ostream& os, Predicate p);
public:
    int op_type; //1:>= 2:<= 3:> 4:< 5:=(num) 6:=(string) 7:join 8:!=(num) 9.!=(string)
    std::string aname; // Name of attribute e.g.customer_rank
    double num;
    std::string str;
    std::vector<std::string> join;  //if op_type = 7 e.g.customer_id order_cid
    void get_interval(std::vector<double>& result);

    Predicate(int op_type, std::string aname, double num);
    Predicate(int op_type, std::string aname, std::string str);
    Predicate(int op_type, std::vector<std::string> join);
    bool test(std::string value);
    bool test(Predicate p);
    std::string to_string() const;
};

#endif