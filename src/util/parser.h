#pragma once

#include <vector>
#include <cstdint>    
#include <cstddef>    
#include <algorithm>  
#include <unordered_map>

enum class RespType {
    SimpleString, 
    SimpleError,
    Integer,
    BulkString,
    Array
};

class MessageParser {
    private: 
    static inline const uint8_t TERMINATOR_ONE = '\r';
    static inline const uint8_t TERMINATOR_TWO = '\n';
    int expectedElements = 0;
    bool PROCESSING_ARRAY = false;
    bool EMPTY_BULK_STRING = false;
    int expectedMessageLength = 0;
    std::vector<uint8_t> destination;
    std::unordered_map<uint8_t, const RespType> respMap = {
        {'+', RespType::SimpleString},
        {'-', RespType::SimpleError},
        {':', RespType::Integer},
        {'$', RespType::BulkString},
        {'*', RespType::Array}
    };

    public:
    uint8_t* getLastPointer(uint8_t* readPtr, size_t length);
    size_t consumeBytes(uint8_t* readPtr, size_t length);
};
