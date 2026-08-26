#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <iostream>
#include "parser.h"
#include "../request.h";

int getValue(std::vector<uint8_t> &destination) {
    int result{0};

    bool isNegative = (destination[1] == '-');
    int startIndex = isNegative ? 2 : 1;

    for (int i{startIndex}; i < destination.size(); ++i)
    {
        result = (result * 10) + (destination[i] - '0');
    }

    return isNegative ? -result : result;
}

uint8_t *MessageParser::getLastPointer(uint8_t *readPtr, size_t length) { // returns the pointer right before the terminator

    if (length >= 2 ) {
        uint8_t *currentPtr = readPtr;

        for (size_t i{0}; i <= length - 2; ++i)
        {
            uint8_t currValue = *(currentPtr + i);
            uint8_t nextValue = *(currentPtr + i + 1);
            if (TERMINATOR_ONE == currValue && TERMINATOR_TWO == nextValue)
            {
                return (currentPtr + i);
            }
        };
    }

    return 0;
}

size_t MessageParser::consumeBytes(uint8_t *readPtr, size_t length) {

    // PROCESS NULL OR EMPTY BULK STRING
    if (EMPTY_BULK_STRING)
    {
        EMPTY_BULK_STRING = false;
        if (PROCESSING_ARRAY) {
            --expectedElements;
            if (expectedElements == 0) { PROCESSING_ARRAY = false; }
        }
        return 2;
    }

    // PROCESSING VALID BULK STRINGS
    if (expectedMessageLength > 0 && length >= (expectedMessageLength + 2))
    {
        int parsedBytes = 0;
        uint8_t *termPtr = readPtr + expectedMessageLength;

        // ensure that terminators are present
        if (TERMINATOR_ONE == *termPtr && TERMINATOR_TWO == *(termPtr + 1))
        {   
            std::string data;
            data.resize(termPtr - readPtr);
            std::copy(readPtr, termPtr, data.begin());
            if (data == "GET" || data == "SET" || data == "DEL") {
                req.setType(data);
            } else {
                req.addArgument(data);                
            }

            if (req.isComplete()) {
                 // SEND COMMAND TO REQUEST HANDLER
                req.reset();
               
            }

            parsedBytes += expectedMessageLength;
            expectedMessageLength = 0; // reset expected message length

            if (PROCESSING_ARRAY)
            {
                --expectedElements;
                if (expectedElements == 0) { PROCESSING_ARRAY = false; }
            }
        }

        return parsedBytes + 2;
    }

    // get the last pointer BEFORE the terminator
    uint8_t *lastPtr = getLastPointer(readPtr, length);
    if (lastPtr == 0) { return 0; }

    //  resize the destination vector
    destination.clear();
    destination.resize(lastPtr - readPtr);
    std::copy(readPtr, lastPtr, destination.begin()); // copy to a temp vector
    std::cout << "Size of destination vector: " << destination.size() << '\n';
    for (int k = 0; k < destination.size(); k++) {
        std::cout << destination[k];
    }
    std::cout << '\n';

    //  find the data type of the message
    uint8_t firstByte = destination[0];
    if (respMap.find(firstByte) == respMap.end())
    {
        exit(EXIT_FAILURE); // TODO: RETURN INVALID DATA TYPE RESPONSE TO CLIENT
    }
    auto respIterator = respMap.find(firstByte);
    RespType respType = respIterator->second;

    //  check the data type of
    if (respType == RespType::Array)
    {                                             // if new array message
        expectedElements = getValue(destination); // number of elements that should be in array
        PROCESSING_ARRAY = true;                  // set the state machine to true
        return lastPtr - readPtr + 2;             // the number of bytes read
    }
    else if (respType == RespType::BulkString)
    {                                                  // if processing bulk string message and still within an array
        expectedMessageLength = getValue(destination); // set the expected message length
        if (expectedMessageLength == 0 ) { EMPTY_BULK_STRING = true; };

        return lastPtr - readPtr + 2;
    }
    else
    {
        // any other response types -> return response is not configured?
    }

    return 0;
}
