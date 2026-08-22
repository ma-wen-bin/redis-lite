#include <vector>
#include <cstdint>    
#include <cstddef>    
#include <algorithm>  
#include <iostream>
#include "parser.h"

    int getValue(std::vector<uint8_t> destination) {
        int result {0};
        for (int i {1}; i < destination.size(); ++i) {
            result = (result * 10) + destination[i];
        }

        return result;
    }

    uint8_t* MessageParser::getLastPointer(uint8_t* readPtr, size_t length) { //returns the pointer right before the terminator 
        uint8_t* currentPtr = readPtr;
        for (size_t i {0}; i < length-1; ++i) {
            uint8_t value = *(currentPtr + i);
            uint8_t nextValue = *(currentPtr + i + 1);
            if (value == firstTerminator && nextValue == secondTerminator) {
                return currentPtr;
            } 
        }

        return 0;
    }

    size_t MessageParser::consumeBytes(uint8_t* readPtr, size_t length) {

        if (expectedMessageLength > 0) { //expecting a bulk string
            uint8_t* lastPtr = readPtr + expectedMessageLength;

            //check that terminators are present 
            if ( firstTerminator == *(lastPtr + 1) && secondTerminator == *(lastPtr + 1)) {
               //TODO: put the data into a string data member of "Command" object
               expectedMessageLength = 0; //reset expected message length
            }

        }

        size_t bytes_parsed = 0;
        uint8_t* lastPtr = getLastPointer(readPtr, length);
        if (lastPtr == 0) {
            return 0; // no terminator found -> return 0 bytes parsed
        }

        //  resize the destination vector 
        destination.clear();
        destination.resize(lastPtr - readPtr);
        std::copy(readPtr, lastPtr+1, destination.begin()); //copy to a temp vector
        std::cout << "Size of destination vector: " << destination.size() << '\n';
        for (int k = 0; k < destination.size(); k++) {
                std::cout << destination[k];
        } 
        
        //  find the data type of the message
        uint8_t firstByte = destination[0]; 
        auto respIterator = respMap.find(firstByte);
        RespType respType = respIterator->second;

        //  check the data type of 
        if (respType == RespType.Array && !processingArray) { // if new array message 
            expectedElements= getValue(destination); // number of elements that should be in array 
            processingArray = true;
            return lastPtr - readPtr; //the number of bytes read
        } else if (respType == RespType.BulkString && processingArray && expectedElements > 0) { //if processing bulk string message and still within an array 
            --expectedElements; //decrement the expected elements
            expectedMessageLength = getValue(destination); //set the expected message length 
            return lastPtr - readPtr;
        } else if (respType == RespType.BulkString && processingArray && expectedElements == 0) { //if processing bulk string message but not within an array 
            //TODO: return error to client 
        } else { //any other response types -> return response is not configured? 

        }

        return bytes_parsed;
    }
