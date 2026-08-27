#include "redis_map.h"

void RedisMap::processRequestQueue(std::queue<Request>& requestQueue) {
    while (!requestQueue.empty()) {
       //PROCESS THE ELEMENTS 
    }
}

void RedisMap::performRequest(const Request &request)
{
    switch (request.getType())
    {
    case RequestType::SET:
        setKeyValue(request);
        break;
    case RequestType::GET:
        getValue(request);
        break;
    case RequestType::DEL:
        deleteValue(request);
        break;
    case RequestType::NONE:
        std::cout << "Request type does not exist. Dropping request!";
    }
}

// SET
void RedisMap::setKeyValue(const Request &request)
{
    if (!request.getKey().empty() && !request.getValue().empty())
    {
        redisMap[request.getKey()] = request.getValue();
        std::cout << "Key: " << request.getKey() << " with value: " << request.getValue() << " was inserted." << '\n';
    }
}

// GET
void RedisMap::getValue(const Request &request)
{
    if (!request.getKey().empty())
    {   
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            std::string value = redisMap[request.getKey()];
            std::cout << "Key: " << request.getKey() << " with value: " << request.getValue() << " was retrieved." << '\n';
            //SEND MESSAGE 
            return;
        }

        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
    }
}

// DEL
void RedisMap::deleteValue(const Request &request)
{
    size_t removed;
    if (!request.getKey().empty())
    {
        removed = redisMap.erase(request.getKey());
    }

    if (removed)
    {
        std::cout << "Key: " << request.getKey() << " was successfully deleted." << '\n';
        return;
    }
    std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
}
