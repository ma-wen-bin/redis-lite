#pragma once

#include <vector>
#include <cstdint>    
#include <cstddef>    
#include <algorithm>  

enum class RespType {
    SimpleString, 
    SimpleError,
    Integer,
    BulkString,
    Array
}

class MessageParser {
    private: 
    static inline const uint8_t firstTerminator = '\r';
    static inline const uint8_t secondTerminator = '\n';
    int expectedElements = 0;
    bool processingArray = false;
    bool processingBulkString = false;
    int messageLength = 0;
    int expectedMessageLength = 0;
    std::unordered_map<uint8_t, const RespType> respMap = {
        {'+', SimpleString},
        {'-', SimpleError},
        {':', Integer},
        {'$', BulkString},
        {'*', Array}
    }
    std::vector<uint8_t> destination;

    public:
    size_t getLastPointer(uint8_t* readPtr, size_t length);
    size_t consumeBytes(uint8_t* readPtr, size_t length);
};
