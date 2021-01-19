#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <vector>
#include <string>
#include <map>

#include "../utils/utils.h"

class Attribute {
private:
    friend std::ostream& operator<<(std::ostream& os, Attribute a);
public:
    const int num_len = 11;
    const int str_len = 20;
    const std::string not_null = "not null";
    std::string aname; // Name of attribute e.g.customer_rank
    bool is_key;
    int type; //1:Integer 2:String
    int value_type;
    //0:Unknown distribution
    //1:ID:no duplicate e.g.1-100
    //2:U:Uniform Distribution e.g.U[1 100]
    //3:PN:Positive Gaussian Distribution e.g.X~N(3,2)and X>0
    //4:Discrete:give corresponding key and proportion.
    std::vector<double> value; //Value for ValueType 1,2,3
    std::map<std::string, double> proportion; //Value for ValueType 4.
    //when type=1(Integer) value_type can still =4

    Attribute(std::string aname, bool is_key, int type);
    Attribute(std::string aname, bool is_key, int type, int value_type, std::vector<double> value);
    Attribute(std::string aname, bool is_key, int type, int value_type, std::map<std::string, double> proportion);

    std::string get_attr_meta();    // get the meta of the attribute(meta data)
};

#endif