#include "Attribute.h"

Attribute::Attribute(std::string aname, bool is_key, int type) : 
aname(lower_string(aname)),  is_key(is_key), type(type) {}

Attribute::Attribute(std::string aname, bool is_key, int type, int value_type, std::vector<double> value) :
aname(lower_string(aname)),  is_key(is_key), type(type), value_type(value_type), value(value) {}

Attribute::Attribute(std::string aname, bool is_key, int type, int value_type, std::map<std::string, double> proportion) :
aname(lower_string(aname)),  is_key(is_key), type(type), value_type(value_type), proportion(proportion) {}

std::ostream& operator<<(std::ostream& os, Attribute a) {
    os << a.aname;
    switch (a.type) {
        case 1:
            os << std::string(" int");
            break;
        case 2:
            os << std::string(" string");
            break;
        default:
            break;
    }
    if(a.is_key)
        os << std::string(" key");
    return os;
}

std::string Attribute::get_attr_meta() {
    std::string res = "";
    res += "`";
    res += this->aname;
    res += "`";
    res += " ";
    switch (this->type) {
        // eg. `id` int(11) not null
        case 1:
            res += "int(";
            res += std::to_string(this->num_len);
            res += ")";
            break;
        // eg. `name` varchar(20) not null
        case 2:
            res += "varchar(";
            res += std::to_string(this->str_len);
            res += ")";
            break;
        default:
            break;
    }
    res += " ";
    res += this->not_null;
    return res;
}
