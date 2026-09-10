#include "redis_map.h"

std::vector<Response> RedisMap::processRequest(const Request& request) {
    performRequest(request);
    return std::move(processedResponses);
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
        case RequestType::SREM:
            srem(request);
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
            break;
    }
}

std::shared_ptr<RedisObject> RedisMap::findKey(const std::string& key) {
    auto iterator = redisMap.find(key);
    if (iterator != redisMap.end()) {
        return iterator->second;
    }
    return nullptr;
}

////////// BASIC OPERATIONS (START)
void RedisMap::get(const Request &request) {
    if (!request.getKey().empty()) {
        auto stringObjectPtr = getChild<RedisObjectString>(findKey(request.getKey()));
        if (stringObjectPtr != nullptr) {
            std::string value = stringObjectPtr->getValue();
            std::cout << "Key: " << request.getKey() << " with value: " << value << " was retrieved." << '\n';
            processedResponses.push_back(Response(ResponseRespType::BulkString, value));
            return;
        }
        processedResponses.push_back(Response(ResponseRespType::Nil));
        std::cout << "Redis object does not exist!" << '\n';
    }
}

void RedisMap::set(const Request &request) {
    if (!request.getKey().empty()) {
        auto stringObjectPtr = getChild<RedisObjectString>(findKey(request.getKey()));
        if (stringObjectPtr != nullptr) {
            stringObjectPtr->setValue(request.getArguments()[1]);
            std::cout << "Key: " << request.getKey() << " updated with value: " << request.getArguments()[1] << '\n';
            return;
        }
        redisMap[request.getKey()] = std::make_shared<RedisObjectString>(request.getArguments()[1]);
        std::cout << "Key: " << request.getKey() << " with value: " << request.getArguments()[1] << " inserted." << '\n';
        processedResponses.push_back(Response(ResponseRespType::SimpleString, "OK"));
    }
}

void RedisMap::del(const Request &request) {
    if (!request.getKey().empty()) {
        auto stringObjectPtr = getChild<RedisObjectString>(findKey(request.getKey()));
        if (stringObjectPtr != nullptr) {
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
        auto stringObjectPtr = getChild<RedisObjectString>(findKey(request.getKey()));
        if (stringObjectPtr != nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Integer, stringObjectPtr->strlen()));
            return;
        }
        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
        processedResponses.push_back(Response(ResponseRespType::Integer, 0)); 
    }
}

void RedisMap::append(const Request &request) {
    if (!request.getKey().empty()) {
        auto stringObjectPtr = getChild<RedisObjectString>(findKey(request.getKey()));
        if (stringObjectPtr != nullptr) {
            stringObjectPtr->append(request.getArguments()[1]);
            processedResponses.push_back(Response(ResponseRespType::Integer, stringObjectPtr->strlen())); 
            return;
        }
        std::cout << "Key: " << request.getKey() << " does not exist." << '\n';
        processedResponses.push_back(Response(ResponseRespType::Integer, 0)); 
    }
}
////////// BASIC OPERATIONS (END)



////////// lIST OPERATIONS (START)
void RedisMap::lpush(const Request &request) {
    if (!request.getKey().empty()) {
        std::vector<std::string> elements(request.getArguments().begin(), request.getArguments().end());
        auto existing = findKey(request.getKey());
        std::shared_ptr<RedisObjectList> listObjectPtr;
        if (existing == nullptr) {
            listObjectPtr = std::make_shared<RedisObjectList>();
            redisMap[request.getKey()] = listObjectPtr;
        } else {
            listObjectPtr = getChild<RedisObjectList>(existing);
        }

        processedResponses.push_back(Response(ResponseRespType::Integer, listObjectPtr->lpush(elements)));
    }
}

void RedisMap::rpush(const Request &request) {
    if (!request.getKey().empty()) {
        std::vector<std::string> elements(request.getArguments().begin(), request.getArguments().end());
        auto existing = findKey(request.getKey());
        std::shared_ptr<RedisObjectList> listObjectPtr;
        if (existing == nullptr) {
            listObjectPtr = std::make_shared<RedisObjectList>();
            redisMap[request.getKey()] = listObjectPtr;
        } else {
            listObjectPtr = getChild<RedisObjectList>(existing);
        }

        processedResponses.push_back(Response(ResponseRespType::Integer, listObjectPtr->rpush(elements)));
    }
}

void RedisMap::lrange(const Request &request) {
    if (!request.getKey().empty()) {
        auto listObjectPtr = getChild<RedisObjectList>(findKey(request.getKey()));
        if (listObjectPtr == nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Array, "[]"));
            return;
        }
        int startIndex = std::stoi(request.getArguments()[1]);
        int stopIndex = std::stoi(request.getArguments()[2]);
        processedResponses.push_back(Response(ResponseRespType::Array, listObjectPtr->lrange(startIndex, stopIndex)));
    }
}
////////// lIST OPERATIONS (END)



////////// HASH OPERATIONS (START)
void RedisMap::hdel(const Request &request) {
    if (!request.getKey().empty()) {
        std::vector<std::string> keys(request.getArguments().begin() + 1, request.getArguments().end());
        auto hashObjectPtr = getChild<RedisObjectHash>(findKey(request.getKey()));
        if (hashObjectPtr != nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Integer, hashObjectPtr->hdel(keys)));
            return;
        }
        processedResponses.push_back(Response(ResponseRespType::Integer, 0));
    }
}

void RedisMap::hset(const Request &request) {
    if (!request.getKey().empty()) {
        auto hashObjectPtr = getChild<RedisObjectHash>(findKey(request.getKey()));
        if (hashObjectPtr != nullptr) {
            std::string key = request.getArguments()[1];
            std::string val = request.getArguments()[2];
            processedResponses.push_back(Response(ResponseRespType::Integer, hashObjectPtr->hset(key, val)));
        }
    }
}

void RedisMap::hget(const Request &request) {
    if (!request.getKey().empty()) {
        auto hashObjectPtr = getChild<RedisObjectHash>(findKey(request.getKey()));
        if (hashObjectPtr != nullptr) {
            std::pair<std::string, int> result = hashObjectPtr->hget(request.getArguments()[1]);
            if (result.first.empty()) {
                processedResponses.push_back(Response(ResponseRespType::Integer, result.second));
                return;
            }  
            processedResponses.push_back(Response(ResponseRespType::BulkString, result.first));
        }
    }
}
////////// HASH OPERATIONS (END)



////////// SET OPERATIONS (START)
void RedisMap::sadd(const Request &request) {
    if (!request.getKey().empty()) {
        auto setObjectPtr = getChild<RedisObjectSet>(findKey(request.getKey()));
        if (setObjectPtr != nullptr) {
            std::vector<std::string> elements(request.getArguments().begin() + 1, request.getArguments().end());
            processedResponses.push_back(Response(ResponseRespType::Integer, setObjectPtr->sadd(elements)));
        }
    }
}

void RedisMap::srem(const Request &request) {
    if (!request.getKey().empty()) {
        auto setObjectPtr = getChild<RedisObjectSet>(findKey(request.getKey()));
        if (setObjectPtr != nullptr) {
            std::vector<std::string> elements(request.getArguments().begin() + 1, request.getArguments().end());
            processedResponses.push_back(Response(ResponseRespType::Integer, setObjectPtr->srem(elements)));
        }
    }
}

void RedisMap::smembers(const Request &request) {
    if (!request.getKey().empty()) {
        auto setObjectPtr = getChild<RedisObjectSet>(findKey(request.getKey()));
        if (setObjectPtr != nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Array, setObjectPtr->smembers()));
        }
    }
}

void RedisMap::scard(const Request &request) {
    if (!request.getKey().empty()) {
        auto setObjectPtr = getChild<RedisObjectSet>(findKey(request.getKey()));
        if (setObjectPtr != nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Integer, setObjectPtr->scard()));
        }
    }
}

void RedisMap::sismember(const Request &request) {
    if (!request.getKey().empty()) {
        auto setObjectPtr = getChild<RedisObjectSet>(findKey(request.getKey()));
        if (setObjectPtr != nullptr) {
            processedResponses.push_back(Response(ResponseRespType::Integer, setObjectPtr->sisMember(request.getArguments()[1])));
        }
    }
}
////////// SET OPERATIONS (END)