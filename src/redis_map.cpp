#include "redis_map.h"

bool RedisMap::isNumber(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    int result;
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
    return ec == std::errc() && ptr == str.data() + str.size();
}

std::vector<Response> RedisMap::processRequestQueue(std::queue<Request>& requestQueue) {
    while (!requestQueue.empty()) {
        performRequest(requestQueue.front());
        requestQueue.pop();
    }
    return processedResponses;
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
        processedResponses.push_back(Response(RespType::SimpleString, "OK"));
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
            std::cout << "Key: " << request.getKey() << " with value: " << value << " was retrieved." << '\n';
            processedResponses.push_back(Response(RespType::BulkString, value));
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
        processedResponses.push_back(Response(RespType::Integer, 1)); //DELTETION TOOK PLACE
        return;
    }
    std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
    processedResponses.push_back(Response(RespType::Integer, 0)); //NO DELETEION TOOK PLACE
}
