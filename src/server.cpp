#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cstddef>
#include "connection.h"
#include "redis_map.h"


using namespace std;

int backlog {10}; //number of connections allowed
struct addrinfo hints; // struct that contains information of the connection 
struct addrinfo *servInfo; // pointer to the results
struct sockaddr_storage clientAddr;
int newSockfd; //new socket file descriptor (client's)
RedisMap map;
std::queue<Request> requestQueue;


int main() {
    
    //SET UP ADDRESS INFORMATION 
    memset(&hints, 0, sizeof(hints)); //ensure no garbage values 
    hints.ai_family = AF_INET; //only ipv4
    hints.ai_socktype = SOCK_STREAM;  //tcp stream sockets 
    hints.ai_flags = AI_PASSIVE; //automatically fill in IP

    int status = getaddrinfo(nullptr, "6380", &hints, &servInfo); 
    if (status != 0) { 
        cout << gai_strerror(status);
        exit(EXIT_FAILURE);
    }

    //CREATE THE SOCKET
    int sockfd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);

    //BIND THE SOCKET
    int bindStatus = bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen);
    if (bindStatus == -1) {
        return errno;
    }
    freeaddrinfo(servInfo); //free the heap allocated linked-lists after binding

    //LISTEN FOR INCOMING CONNECTIONS 
    int listenStatus = listen(sockfd, backlog);

    //ACCEPT INCOMING CONNECTIONS 
    socklen_t addr_size = sizeof(clientAddr);
    newSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
    if (newSockfd == -1) {
        return errno;
    }
    Connection connection = Connection(newSockfd);
    
    //RECEIVE INCOMING MESSAGES
    while(true) {
        IncomingMessage incomingMessage = connection.processIncomingMessage();

        if (incomingMessage.inboundRequests.empty()) {
            continue;
        }
        
        for (Request inboundRequest : incomingMessage.inboundRequests) {
            std::cout << "Processing inbound requests" << '\n';
            requestQueue.push(inboundRequest);
        }

        map.processRequestQueue(requestQueue);

        if (!incomingMessage.clientStatus) {
            std::cout << "Closing client socket file descriptor." << '\n';
            close(newSockfd);
            break;
        }
        
    }

    close(sockfd); 
    return 0;
} 