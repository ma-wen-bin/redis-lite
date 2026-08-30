#include "util/ring_buffer.h"
#include "util/parser.h"
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
    void processOutgoingMessage();
    void enqueueResponseMesage(const std::vector<uint8_t>& responseBytes);

};