#pragma once
#include <unordered_map>
#include <string>
#include <queue>
#include <charconv>
#include <memory>

#include "request.h"
#include "response.h"
#include "redis_obj.h"


class RedisMap {
    private:
    inline static std::unordered_map<std::string, std::shared_ptr<RedisObject>> redisMap;
    std::vector<Response> processedResponses; 

    public:
    std::vector<Response> processRequestQueue(std::queue<Request>& requestQueue);

    // BASE HANDLER
    void performRequest(const Request &request);

    // BASIC OPERATIONS 
    void get(const Request &request);
    void set(const Request &request);
    void del(const Request &request);
    void strlen(const Request &request);
    void append(const Request &request);


    // LIST OPERATIONS
    void lpush(const Request &request);
    void rpush(const Request &request);
    void lrange(const Request &request);

    // HASH OPERATIONS
    void hdel(const Request &request);
    void hset(const Request &request);
    void hget(const Request &request);
    
    // SET OPERATIONS
    void sadd(const Request &request);
    void srem(const Request &request);
    void smembers(const Request &request);
    void scard(const Request &request);
    void sismember(const Request &request);

};