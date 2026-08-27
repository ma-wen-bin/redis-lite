#include "connection.h"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include "request.h"


// PROCESS INCOMING MESSAGES
IncomingMessage Connection::processIncomingMessage()
{   
    IncomingMessage incomingMessage;

    if (incomingBuffer.isFull())
    {
        // peek to view all available data
        const auto &[readPtr, readLen] = incomingBuffer.peek();
        ParsedMessage parsedMessage = parser.consumeBytes(readPtr, readLen);
        //check if parsedMessage contains a Request object -> add it to a vector to be returned
        
        incomingBuffer.consume(parsedMessage.parsedBytes);
        if (parsedMessage.req) { // if parsedMessage contains Request object
            incomingMessage.inboundRequests.push_back(*parsedMessage.req);
        }

        if (parsedMessage.parsedBytes == 0)
        { // no delimiter found -> close connection
            std::cout << "Message exceeds buffer size, closing connection." << '\n';
            exit(EXIT_FAILURE);
        }
    }

    const auto &[writePtr, writeLen] = incomingBuffer.writeableSpan();  // get the starting point and length that is empty
    size_t bytes_read = recv(clientSocketFD, writePtr, writeLen, 0); // write to the buffer

    if (bytes_read == -1)
    {
        std::cout << "Read error : " << std::strerror(errno) << '\n';
        return incomingMessage;
    }

    if (bytes_read == 0)
    {
        while (incomingBuffer.size() > 0)
        {
            std::cout << "Client disconnected. Parsing the remaining buffer" << '\n';
            const auto &[readPtr, readLen] = incomingBuffer.peek();
            ParsedMessage parsedMessage = parser.consumeBytes(readPtr, readLen);
            if (parsedMessage.req) {
                incomingMessage.inboundRequests.push_back(*parsedMessage.req);
            }
            if (parsedMessage.parsedBytes == 0) {   break;  }
            incomingBuffer.consume(parsedMessage.parsedBytes);
            std::cout << "Parser has consumed: " << parsedMessage.parsedBytes << '\n';
        }
        
        incomingMessage.clientStatus = false;
        return incomingMessage;
    }

    std::cout << "Current buffer size: " << incomingBuffer.size() << '\n';
    std::cout << "Writeable length: " << writeLen << '\n';
    std::cout << "Read :" << bytes_read << " bytes" << '\n';
    incomingBuffer.commitWrite(bytes_read); // update the head pointer's position

    const auto &[readPtr, readLen] = incomingBuffer.peek();
    std::cout << "Peeked: " << readLen << '\n';
    ParsedMessage parsedMessage = parser.consumeBytes(readPtr, readLen);
    if (parsedMessage.req) {
        incomingMessage.inboundRequests.push_back(*parsedMessage.req);
    }
    
    if (parsedMessage.parsedBytes > 0)
    {
        incomingBuffer.consume(parsedMessage.parsedBytes);
        std::cout << "Parser has consumed: " << parsedMessage.parsedBytes << '\n';
    }

    return incomingMessage;
}

// PROCESS OUTGOING MESSAGES 
void Connection::processOutgoingMessage() {
    if (outgoingBuffer.isFull()) {

    }
}