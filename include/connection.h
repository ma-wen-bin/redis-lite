#pragma once
#include <sys/types.h> 
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include "ring_buffer.h"
#include "parser.h"
#include "request.h"
#include "response.h"

struct IncomingMessage {
    bool clientStatus {true};
    std::vector<Request> inboundRequests;
};

class Connection {
    private:
    RingBuffer incomingBuffer;
    RingBuffer outgoingBuffer;
    int clientSocketFD;
    MessageParser parser;
    std::vector<Request> inboundRequests;

    public:
    Connection(int clientSockfd) : clientSocketFD(clientSockfd), incomingBuffer(8192), outgoingBuffer(8192) {};
    IncomingMessage processIncomingMessage();
    ssize_t processOutgoingMessage(); // -1 = would block (caller should watch EPOLLOUT), else fully drained
    void enqueueResponseMessage(const std::vector<uint8_t>& responseBytes);

};