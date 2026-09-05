#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>

enum class RedisObjectType {
    String, 
    List,
    Hash,
    Set
};

class RedisObject {
    private: 
    RedisObjectType type;

    public:
    virtual ~RedisObject() = default; // destructor for child classes
    RedisObject(RedisObjectType t): type(t) {}; 
    RedisObjectType getType() const;
};


// STRING (child)
class RedisObjectString : public RedisObject {
    private:
    std::string value;

    public:
    std::pair<std::string, int> getValue();
    int strlen();
    int append(const std::string& value);
    
    RedisObjectString() : RedisObject(RedisObjectType::String) {};
    RedisObjectString(const std::string& val) : RedisObject(RedisObjectType::String), value(val) {};
};

// LIST (child)
class RedisObjectList : public RedisObject {
    private:
    std::deque<std::string> list;

    public:
    int lpush(const std::vector<std::string>& elements); //returns the length of the list after the push operation.
    int rpush(const std::vector<std::string>& elements); //returns the length of the list after the push operation.
    std::vector<std::string> lrange(int startIndex, int stopIndex); //a list of elements in the specified range, or an empty array if the key doesn't exist.

    RedisObjectList() : RedisObject(RedisObjectType::List) {};
    RedisObjectList(const std::deque<std::string>& l) : RedisObject(RedisObjectType::List), list(l) {};
};

// HASH (child)
class RedisObjectHash : public RedisObject {
    private:
    std::unordered_map<std::string, std::string> hashMap;
    
    public:
    int hdel(const std::string& key); //returns the number of fields that were removed from the hash, excluding any specified but non-existing fields.
    int hset(const std::string& key, const std::string value); //returns the number of fields that were added.
    std::pair<std::string, int> hget(const std::string& key); //returns the value associated with the field OR '-1' if the field is not present in the hash or key does not exist.

    RedisObjectHash() : RedisObject(RedisObjectType::Hash) {};
    RedisObjectHash(const std::unordered_map<std::string, std::string> m) : RedisObject(RedisObjectType::Hash), hashMap(m) {};
};

// SET (child)
class RedisObjectSet : public RedisObject {
    private:
    std::unordered_set<std::string> set;
    
    public:
    int sadd(const std::vector<std::string>& elements); //returns the number of elements that were added to the set, not including all the elements already present in the set.
    int srem(const std::vector<std::string>& elements); //returns the number of members that were removed from the set, not including non existing members.
    std::vector<std::string>& smembers() const; //returns an array with all the members of the set.
    int scard(); //returns the cardinality (number of elements) of the set, or 0 if the key does not exist.
    int isMember(const std::string& element); //returns 0 if the element is not a member of the set, or when the key does not exist OR 1 if the element is a member of the set.

    RedisObjectSet() : RedisObject(RedisObjectType::Set) {};
    RedisObjectSet(const std::unordered_set<std::string>& s) : RedisObject(RedisObjectType::Set), set(s) {};
};