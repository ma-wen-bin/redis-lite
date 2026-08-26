#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

enum class RequestType {
    GET, 
    SET,
    DEL,
    NONE
};


class Request {
    private:
    RequestType type;
    std::vector<std::string> args; 
    inline static const std::unordered_map<std::string, RequestType> requestTypeMap {
    {"GET", RequestType::GET},
    {"SET", RequestType::SET},
    {"DEL", RequestType::DEL}
    };


    public:
    Request() : type(RequestType::NONE) {};
    Request(RequestType& t) : type(t) {
        std::cout << "Initialised new request object" << '\n';
    };
    void setType(const RequestType& t);
    void setType(std::string& t);
    void addArgument(std::string arg);
    int getArgumentSize();
    std::string getKey() const;
    std::string getValue() const;
    RequestType getType() const;
    std::string getStringType() const;
    bool isComplete() const;
    void reset();

};