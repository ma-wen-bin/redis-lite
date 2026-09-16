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
            incomingMessage.clientStatus = false;
            return incomingMessage;
        }
    }

    const auto &[writePtr, writeLen] = incomingBuffer.writeableSpan();  // get the starting point and length that is empty
    ssize_t bytes_read = recv(clientSocketFD, writePtr, writeLen, 0); // write to the buffer

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

    while (true) {

        const auto &[readPtr, readLen] = incomingBuffer.peek();
        std::cout << "Peeked: " << readLen << '\n';
        ParsedMessage parsedMessage = parser.consumeBytes(readPtr, readLen);
        if (parsedMessage.req) {
            if (parsedMessage.req.value().isFatal()) {
                incomingMessage.clientStatus = false;
            } 
            incomingMessage.inboundRequests.push_back(*parsedMessage.req);
        }

        if (parsedMessage.parsedBytes == 0) {
            std::cout << "Parsed bytes is 0" << '\n';
            if (readLen < incomingBuffer.size()) {
                std::cout << "Shifting" << '\n';
                incomingBuffer.shift();
                continue;
            }
            break;            
        }

        incomingBuffer.consume(parsedMessage.parsedBytes);
        std::cout << "Parser has consumed: " << parsedMessage.parsedBytes << '\n';

    }

    return incomingMessage;
}


void Connection::enqueueResponseMessage(const std::vector<uint8_t>& responseBytes) {

    size_t offset = 0;
    while (offset < responseBytes.size()) {
        if (outgoingBuffer.isFull()) {
            processOutgoingMessage();
        }

        const auto &[writePtr, writeLen] = outgoingBuffer.writeableSpan();
        size_t chunk = std::min(writeLen, responseBytes.size() - offset);
        if (chunk == 0) processOutgoingMessage();
        outgoingBuffer.insert(responseBytes, offset, chunk);
        offset += chunk;
    }

}

ssize_t Connection::processOutgoingMessage() {
    std::cout << "outgoing buffer size: " << outgoingBuffer.size() << '\n';
    ssize_t totalSent = 0;

    while (!outgoingBuffer.isEmpty()) {
        const auto &[readPtr, readLen] = outgoingBuffer.peek(); // only the contiguous run up to the wrap point
        ssize_t bytes_sent = send(clientSocketFD, readPtr, readLen, 0);

        if (bytes_sent == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1; 
            }
            std::cout << "Send error : " << std::strerror(errno) << '\n';
            return -1;
        }

        std::cout << "Sent " << bytes_sent << " bytes to client" << '\n';
        outgoingBuffer.consume(bytes_sent); 
        totalSent += bytes_sent;
    }

    return totalSent; 
}