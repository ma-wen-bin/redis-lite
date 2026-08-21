#pragma once

#include <vector>
#include <cstdint>    
#include <cstddef>    
#include <algorithm>  

class MessageParser {
    private: 
    std::vector<uint8_t> destination;

    public:
    size_t consumeBytes(uint8_t* readPtr, size_t length);
};
