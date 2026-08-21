#include <vector>
#include <cstdint>    
#include <cstddef>    
#include <algorithm>  
#include <iostream>
#include "parser.h"

    size_t MessageParser::consumeBytes(uint8_t* readPtr, size_t length) {
        size_t bytes_parsed = 0;
        uint8_t* currentPtr = readPtr;

        for (size_t i {0}; i < length; ++i) {
            uint8_t value = *(currentPtr + i);
            if (value == '\n') { 
                destination.clear();
                destination.resize((currentPtr+i) - readPtr);

                std::copy(readPtr, currentPtr+i, destination.begin()); //copy to a temp vector
                std::cout << "Size of destination vector: " << destination.size() << '\n';
                for (int k = 0; k < destination.size(); k++) {
                    std::cout << destination[k];
                } 
                std::cout << '\n';
                bytes_parsed = (currentPtr+i+1) - readPtr; //calculate the bytes parsed 
                break;
            }
        }

        return bytes_parsed;
    }
