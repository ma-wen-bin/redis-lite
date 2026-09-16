#include "redis_obj.h"

//BASE METHOD 
RedisObjectType RedisObject::getType() const {
    return type;
}

//STRING METHODS
void RedisObjectString::setValue(const std::string& val) {
    value = val;
}

std::string RedisObjectString::getValue() const {
    return value;
}

int RedisObjectString::strlen() const {
    return value.length();
}

int RedisObjectString::append(const std::string& additionalValue) {
    value += additionalValue;
    return value.length();
}

//LIST METHODS 
int RedisObjectList::lpush(const std::vector<std::string>& elements) {
    for (std::string element : elements) {
        list.push_front(element);
    }
    return list.size();
}

int RedisObjectList::rpush(const std::vector<std::string>& elements) {
    for (std::string element : elements) {
        list.push_back(element);
    }
    return list.size();
}

std::vector<std::string> RedisObjectList::lrange(int startIndex, int stopIndex) {
    int size = static_cast<int>(list.size());
    if (size == 0) return {};

    if (startIndex < 0) startIndex = std::max(size + startIndex, 0);
    if (stopIndex < 0) stopIndex = size + stopIndex; 

    if (startIndex >= size || stopIndex < 0 || startIndex > stopIndex) {
        return {};
    }

    stopIndex = std::min(stopIndex, size - 1);

    return std::vector<std::string>(list.begin() + startIndex, list.begin() + stopIndex + 1);
}

//HASH METHODS 
int RedisObjectHash::hdel(const std::vector<std::string>& keys) {
    int total {0};
    for (std::string key : keys) {
        auto iterator = hashMap.find(key);
        if (iterator == hashMap.end()) continue;
        hashMap.erase(key);
        ++total;
    }
    return total;
}

int RedisObjectHash::hset(const std::string& key, const std::string& value) {
    if (hashMap.find(key) != hashMap.end()) {
        hashMap[key] = value;
        return 0;
    }
    hashMap[key] = value;
    return 1;
}

std::pair<std::string, int> RedisObjectHash::hget(const std::string& key) const {
    auto iterator = hashMap.find(key);  
    if (iterator == hashMap.end()) return {"", -1};
    return {iterator->second, 1};
}

//SET METHODS 
int RedisObjectSet::sadd(const std::vector<std::string>& elements) {
    int total {0};
    for (std::string element : elements) {
        auto[iterator, succcess] = set.insert(element);
        if (succcess) ++total;
    }
    return total;
}

int RedisObjectSet::srem(const std::vector<std::string>& elements) {
    int total {0};
    for (std::string element : elements) {
        if (set.erase(element)) {
            ++total;
        }
    }
    return total;
}

std::vector<std::string> RedisObjectSet::smembers() const {
    return std::vector<std::string>(set.begin(), set.end());
}

int RedisObjectSet::scard() const {
    return set.size();
}

int RedisObjectSet::sisMember(const std::string& element) const {
    if (set.find(element) != set.end()) return 1;
    return 0;
}
