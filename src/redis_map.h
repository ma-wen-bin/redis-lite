#pragma once
#include <unordered_map>
#include <string>
#include "request.h"


class RedisMap {
    private:
    inline static std::unordered_map<std::string, std::string> redisMap;

    public:
    void performRequest(const Request &request);
    void setKeyValue(const Request &request);
    void getValue(const Request &request);
    void deleteValue(const Request &request);

};