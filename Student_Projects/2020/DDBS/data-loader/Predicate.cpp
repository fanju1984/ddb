#include "Predicate.h"

Predicate::Predicate(int op_type, std::string aname, double num) :
op_type(op_type), aname(lower_string(aname)), num(num) {}

Predicate::Predicate(int op_type, std::string aname, std::string str) :
op_type(op_type), aname(lower_string(aname)), str(str) {}

Predicate::Predicate(int op_type, std::vector<std::string> join) :
op_type(op_type), join(join) {}

std::ostream& operator<<(std::ostream& os, Predicate p) {
    switch (p.op_type) {
        case 1:
            os << p.aname << std::string(" >= ") << double2string(p.num);
            break;
        case 2:
            os << p.aname << std::string(" <= ") << double2string(p.num);
            break;
        case 3:
            os << p.aname << std::string(" > ") << double2string(p.num);
            break;
        case 4:
            os << p.aname << std::string(" < ") << double2string(p.num);
            break;
        case 5:
            os << p.aname << std::string(" = ") << double2string(p.num);
            break;
        case 6:
            os << p.aname << std::string(" = '") << p.str << std::string("'");
            break;
        case 7:
            os << p.join[0] << std::string(" = ") << p.join[1];
            break;
        case 8:
            os << p.aname << std::string(" != ") << double2string(p.num);
            break;
        case 9:
            os << p.aname << std::string(" != '") << p.str << std::string("'");
            break;
        default:
            break;
    }
    return os;
}

bool Predicate::test(std::string value) {
    switch(this->op_type) {
        case 1:
            return std::stod(value) >= this->num;
        case 2:
            return std::stod(value) <= this->num;
        case 3:
            return std::stod(value) > this->num;
        case 4:
            return std::stod(value) < this->num;
        case 5:
            return std::stod(value) == this->num;
        case 6:
            return value == this->str || value == "'" + this->str + "'";
        case 8:
            return std::stod(value) != this->num;
        case 9:
            return value != this->str && value != "'" + this->str + "'";
        default:
            break;
    }
}

bool Predicate::test(Predicate p) {
    if(this->aname == p.aname) {
        switch(this->op_type) {
            case 1:
                if((p.op_type==1) || (p.op_type==2&&this->num<=p.num) || (p.op_type==3) || (p.op_type==4&&this->num<p.num) || (p.op_type==5&&this->num<=p.num) || (p.op_type==8)) {
                    return true;
                } else {
                    return false;
                }
            case 2:
                if((p.op_type==1&&this->num>=p.num) || (p.op_type==2) || (p.op_type==3&&this->num>p.num) || (p.op_type==4) || (p.op_type==5&&this->num>=p.num) || (p.op_type==8)) {
                    return true;
                } else {
                    return false;
                }
            case 3:
                if((p.op_type==1) || (p.op_type==2&&this->num<p.num) || (p.op_type==3) || (p.op_type==4&&this->num<p.num) || (p.op_type==5&&this->num<p.num) || (p.op_type==8)) {
                    return true;
                } else {
                    return false;
                }
            case 4:
                if((p.op_type==1&&this->num>p.num) || (p.op_type==2) || (p.op_type==3&&this->num>p.num) || (p.op_type==4) || (p.op_type==5&&this->num>p.num) || (p.op_type==8)) {
                    return true;
                } else {
                    return false;
                }
            case 5:
                if((p.op_type==1&&this->num>=p.num) || (p.op_type==2&&this->num<=p.num) || (p.op_type==3&&this->num>p.num) || (p.op_type==4&&this->num<p.num) || (p.op_type==5&&this->num==p.num) || (p.op_type==8&&this->num!=p.num)) {
                    return true;
                } else {
                    return false;
                }
            case 6:
                if((p.op_type==6&&this->str==p.str) || (p.op_type==9&&this->str!=p.str)) {
                    return true;
                } else {
                    return false;
                }
            case 8:
                if(p.op_type==5&&this->num==p.num) {
                    return false;
                } else {
                    return true;
                }
            case 9:
                if(p.op_type==6&&this->str==p.str) {
                    return false;
                } else {
                    return true;
                }
            default:
                break;
        }
    }
    return true;
}

std::string Predicate::to_string() const {
    std::string result = "";
    switch (this->op_type) {
        case 1:
            result = this->aname + std::string(" >= ") + double2string(this->num);
            break;
        case 2:
            result = this->aname + std::string(" <= ") + double2string(this->num);
            break;
        case 3:
            result = this->aname + std::string(" > ") + double2string(this->num);
            break;
        case 4:
            result = this->aname + std::string(" < ") + double2string(this->num);
            break;
        case 5:
            result = this->aname + std::string(" = ") + double2string(this->num);
            break;
        case 6:
            result = this->aname + std::string(" = '") + this->str + std::string("'");
            break;
        case 7:
            result = this->join[0] + std::string(" = ") + this->join[1];
            break;
        case 8:
            result = this->aname + std::string(" != ") + double2string(this->num);
            break;
        case 9:
            result = this->aname + std::string(" != '") + this->str + std::string("'");
            break;
        default:
            break;
    }
    return result;
}
