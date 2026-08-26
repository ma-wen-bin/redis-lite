#include <string>
#include <iostream>
#include <stdexcept>
#include "util/parser.h"

enum class RequestType {
    GET, 
    SET,
    DEL,
    NONE
};

static std::unordered_map<std::string, RequestType> requestTypeMap {
    {"GET", RequestType::GET},
    {"SET", RequestType::SET},
    {"DEL", RequestType::DEL}
};

class Request {
    private:
    RequestType type;
    std::vector<std::string> args; 


    public:
    Request() {};
    Request(RequestType& t) : type(t) {};
    void setType(const RequestType& t);
    void setType(std::string& t);
    void addArgument(std::string arg);
    int getArgumentSize();
    std::string getKey() const;
    std::string getValue() const;
    RequestType getType() const;
    bool isComplete() const;
    void reset();

};