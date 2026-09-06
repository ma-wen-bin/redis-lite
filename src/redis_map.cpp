#include "redis_map.h"

std::vector<Response> RedisMap::processRequestQueue(std::queue<Request>& requestQueue) {
    while (!requestQueue.empty()) {
        performRequest(requestQueue.front());
        requestQueue.pop();
    }
    return processedResponses;
}

void RedisMap::performRequest(const Request &request)
{   
    switch (request.getType()) {
        
        // BASIC OPERATIONS
        case RequestType::SET: 
            set(request);
            break;
        case RequestType::GET: 
            get(request);
            break;
        case RequestType::DEL: 
            del(request);
            break;
        case RequestType::STRLEN: 
            strlen(request);
            break;    
        case RequestType::APPEND: 
            append(request);
            break;        
        
        // LIST OPERATIONS
        case RequestType::LPUSH: 
            lpush(request);
            break;
        case RequestType::RPUSH: 
            rpush(request);
            break;
        case RequestType::LRANGE: 
            lrange(request);
            break;
        
        // HASH OPERATIONS
        case RequestType::HDEL: 
            hdel(request);
            break;
        case RequestType::HSET: 
            hset(request);
            break;
        case RequestType::HGET: 
            hget(request);
            break;

        // SET OPERATIONS
        case RequestType::SADD: 
            sadd(request);
            break;
        case RequestType::SMEMBERS: 
            smembers(request);
            break;
        case RequestType::SCARD: 
            scard(request);
            break;            
        case RequestType::SISMEMBER: 
            sismember(request);
            break;   

        case RequestType::NONE:
            std::cout << "Request type does not exist. Dropping request!";    
    }
}


////////// BASIC OPERATIONS (START)
void RedisMap::get(const Request &request) {
    if (!request.getKey().empty()) {
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            std::shared_ptr<RedisObjectString> stringObjectPtr = std::dynamic_pointer_cast<RedisObjectString>(redisMap[request.getKey()]);
            if (stringObjectPtr != nullptr) {
                std::string value = stringObjectPtr->getValue();
                std::cout << "Key: " << request.getKey() << " with value: " << value << " was retrieved." << '\n';
                processedResponses.push_back(Response(ResponseRespType::BulkString, value));
            }
            std::cout << "Redis object retireved does not exist!" << '\n';
        }
    }
}

void RedisMap::set(const Request &request) {
    if (!request.getKey().empty()) {
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            std::shared_ptr<RedisObjectString> stringObjectPtr = std::dynamic_pointer_cast<RedisObjectString>(redisMap[request.getKey()]);
            if (stringObjectPtr != nullptr) {
                stringObjectPtr->setValue(request.getArguments()[1]);
                std::cout << "Key: " << request.getKey() << " updated with value: " << request.getArguments()[1] << '\n';
            }
        }

        redisMap[request.getKey()] = std::make_shared<RedisObjectString>(request.getArguments()[1]);
        std::cout << "Key: " << request.getKey() << " with value: " << request.getArguments()[1] << " inserted." << '\n';
        processedResponses.push_back(Response(ResponseRespType::SimpleString, "OK"));
    }
}

void RedisMap::del(const Request &request) {
    if (!request.getKey().empty()) {
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            redisMap.erase(request.getKey());
            std::cout << "Key: " << request.getKey() << " was successfully deleted." << '\n';
            processedResponses.push_back(Response(ResponseRespType::Integer, 1)); 
            return;
        }
        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
        processedResponses.push_back(Response(ResponseRespType::Integer, 0)); 
    }
}

void RedisMap::strlen(const Request &request) {
    if (!request.getKey().empty()) {
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            std::shared_ptr<RedisObjectString> stringObjectPtr = std::dynamic_pointer_cast<RedisObjectString>(redisMap[request.getKey()]);
            processedResponses.push_back(Response(ResponseRespType::Integer, stringObjectPtr->strlen())); 
            return;
        }
        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
        processedResponses.push_back(Response(ResponseRespType::Integer, 0)); 
    }
}

void RedisMap::append(const Request &request) {
    if (!request.getKey().empty()) {
        auto iterator = redisMap.find(request.getKey());
        if (iterator != redisMap.end()) {
            std::shared_ptr<RedisObjectString> stringObjectPtr = std::dynamic_pointer_cast<RedisObjectString>(redisMap[request.getKey()]);
            stringObjectPtr->append(request.getArguments()[2]);
            processedResponses.push_back(Response(ResponseRespType::Integer, stringObjectPtr->strlen())); 
            return;
        }
        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
        processedResponses.push_back(Response(ResponseRespType::Integer, 0)); 
    }
}

////////// BASIC OPERATIONS (END)


