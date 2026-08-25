#include <string>
#include <iostream>
#include <stdexcept>
#include "util/parser.h"


enum class RequestType {
    GET, 
    SET,
    DEL
};



class Request {
    private:
    RequestType type;
    std::vector<std::string> args; 

    public:
    void setType(const RequestType& t) {
        type = type;
    }

    void addArgument(std::string arg) {
        args.emplace_back(arg);
    }

    std::string getKey() const {
        return (args.size() > 0) ? args[0] : "";
    }

    std::string getValue() const{
        return (args.size() > 1) ? args[1] : "";
    }
    
    RequestType getType() const { return type; }

    bool isComplete() const {
        if (type == RequestType::SET) return args.size() == 2;
        if (type == RequestType::GET || type == RequestType::DEL) return args.size() == 1;
    }

    // void setRequest(RequestType& type, const std::string& key, const std::string& value) {
    //     if (type != RequestType::SET) {
    //         std::cerr << "Wrong type" << '\n';
    //         return;
    //     }

    //     args.clear();
    //     args.push_back(key);
    //     args.push_back(value);
    // }

    // void setRequest(RequestType& type, std::string key) {
    //     if (type == RequestType::GET || type == RequestType::DEL) {
    //         args.clear();
    //         args.push_back(key);
    //     };
    // }

};