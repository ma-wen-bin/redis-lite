#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include "request.h"
#include "response.h"

class RedisMap {
    private:
    inline static std::unordered_map<std::string, std::string> redisMap;
    std::vector<Response> response; 

    public:
    std::vector<Response> processRequestQueue(std::queue<Request>& requestQueue);
    void performRequest(const Request &request);
    void setKeyValue(const Request &request);
    void getValue(const Request &request);
    void deleteValue(const Request &request);

};