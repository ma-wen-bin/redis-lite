#pragma once

#include <string>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <climits>

enum class RequestType {
    GET, 
    SET,
    DEL,
    STRLEN,
    APPEND,
    LPUSH,
    RPUSH,
    LRANGE,
    HDEL,
    HSET,
    HGET,
    SADD,
    SREM,
    SMEMBERS,
    SCARD,
    SISMEMBER,
    NONE
};

struct CommandSpec {
    RequestType type;
    int minArgs;
    int maxArgs;
};

inline static const std::unordered_map<std::string, CommandSpec> commandTable {
    {"GET", {RequestType::GET, 1, 1}},
    {"SET", {RequestType::SET, 2, 2}},
    {"DEL", {RequestType::DEL, 1, INT_MAX}},
    {"STRLEN", {RequestType::STRLEN, 1, 1}},
    {"APPEND", {RequestType::APPEND, 2, 2}},
    {"LPUSH", {RequestType::LPUSH, 2, INT_MAX}},
    {"RPUSH", {RequestType::RPUSH, 2, INT_MAX}},
    {"LRANGE", {RequestType::LRANGE, 3, 3}},
    {"HDEL", {RequestType::HDEL, 3, INT_MAX}},
    {"HSET", {RequestType::HSET, 3, 3}},
    {"HGET", {RequestType::HGET, 2, 2}},
    {"SADD", {RequestType::SADD, 2, INT_MAX}},
    {"SREM", {RequestType::SREM, 2, INT_MAX}},
    {"SMEMBERS", {RequestType::SMEMBERS, 1, 1}},
    {"SCARD", {RequestType::SCARD, 1, 1}},
    {"SISMEMBER", {RequestType::SISMEMBER, 2, 2}},
};

class Request {
    private:
    const CommandSpec* spec = nullptr;
    RequestType type;
    std::string commandName;
    std::string errorMessage;
    std::vector<std::string> args;

    public:
    Request() : type(RequestType::NONE) {};
    Request(RequestType& t) : type(t) {
        std::cout << "Initialised new request object" << '\n';
    };
    bool setCommand(const std::string& token);
    void addArgument(std::string arg);
    void setError(std::string message);
    bool hasError() const;
    const std::string& getError() const;
    const std::string& getCommandName() const;
    std::string getKey() const;
    std::vector<std::string> getArguments() const;
    RequestType getType() const;
    bool isComplete() const;
    void reset();

};