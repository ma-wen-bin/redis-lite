#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h> 
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cstddef>
#include "connection.h"
#include "redis_map.h"

int backlog {10}; //number of connections allowed
struct addrinfo hints; // struct that contains information of the connection 
struct addrinfo *servInfo; // pointer to the results
struct sockaddr_storage clientAddr;
int newSockfd; //new socket file descriptor (client's)
const int MAX_EVENTS {5};
RedisMap map;
std::queue<Request> requestQueue;
std::queue<Response> responseQueue;
std::unordered_map<int, Connection> connectionMap;

struct QueuedRequest {
    Request req;
    int socketFd;
};

void processMessage(Connection& connection, int clientSocketfd, int epollfd) {

    IncomingMessage incomingMessage = connection.processIncomingMessage();

    if (incomingMessage.inboundRequests.empty()) {
        return;
    }

    for (Request inboundRequest : incomingMessage.inboundRequests) {
        requestQueue.push(inboundRequest);
    }

    while (!requestQueue.empty()) {
        std::vector<Response> responses = map.processRequest(requestQueue.front());
        requestQueue.pop();
        for (Response response : responses) {
            connection.enqueueResponseMessage(response.serialize());
        }
        if (connection.processOutgoingMessage() == -1) {
            epoll_event event;
            event.events = EPOLLIN | EPOLLOUT;
            event.data.fd = clientSocketfd;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, clientSocketfd, &event);
        };
    }

    // HANDLE CLIENT DISCONNECT 
    if (!incomingMessage.clientStatus) {
        std::cout << "Closing client socket file descriptor." << '\n';
        connectionMap.erase(clientSocketfd);
        epoll_ctl(epollfd, EPOLL_CTL_DEL, clientSocketfd, nullptr);
        close(clientSocketfd);
        return;
    }

}

int main() {
    
    //DISABLE COUT 
    //std::cout.rdbuf(nullptr);

    //SET UP ADDRESS INFORMATION 
    memset(&hints, 0, sizeof(hints)); //ensure no garbage values 
    hints.ai_family = AF_INET; //only ipv4
    hints.ai_socktype = SOCK_STREAM;  //tcp stream sockets 
    hints.ai_flags = AI_PASSIVE; //automatically fill in IP

    int status = getaddrinfo(nullptr, "6380", &hints, &servInfo); 
    if (status != 0) { 
        std::cout << gai_strerror(status);
        exit(EXIT_FAILURE);
    }

    //CREATE THE SOCKET
    int sockfd = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);

    //CREATE THE EPOLL FILE DESCRIPTOR 
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    int epollfd = epoll_create1(0);

    if (epollfd == -1) {
        std::cout << "Failed to create epoll file descriptor" << '\n';
        return -1;
    }

    event.events = EPOLLIN; // means that associated fd is ready for read operation

    //BIND THE SOCKET
    int bindStatus = bind(sockfd, servInfo->ai_addr, servInfo->ai_addrlen);
    const int optval = 1;
    setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    if (bindStatus == -1) {
        std::cout << "Failed to bind: " << strerror(errno) << '\n';
        return errno;
    }
    freeaddrinfo(servInfo); //free the heap allocated linked-lists after binding

    //LISTEN FOR INCOMING CONNECTIONS + SET LISTENING SOCKET AS NON-BLOCKING 
    int listenStatus = listen(sockfd, backlog);
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    // ADD LISTENING SOCKET TO EPOLL INSTANCE
    event.data.fd = sockfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &event)) {
        std::cout << "Failed to add listening socket file descriptor to epoll" << '\n';
        close(epollfd);
        return -1;
    }

    while (true) {
        int eventCount = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        for (int i {0}; i < eventCount; ++i) {
            // READ OPERATION IS READY FROM THE LISTENING SOCKET
            if (events[i].data.fd == sockfd) {
                while (true) {
                    socklen_t addr_size = sizeof(clientAddr);
                    newSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &addr_size);
                    if (newSockfd == -1) break; 

                    int flags = fcntl(newSockfd, F_GETFL, 0);
                    fcntl(newSockfd, F_SETFL, flags | O_NONBLOCK);

                    connectionMap.emplace(newSockfd, Connection(newSockfd));
                    epoll_event clientEvent{};
                    clientEvent.events = EPOLLIN;
                    clientEvent.data.fd = newSockfd;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newSockfd, &clientEvent)) {
                        std::cout << "Failed to add client socket file descriptor to epoll" << '\n';
                        close(newSockfd);
                        continue;
                    }
                }
                continue;
            }

            if (events[i].events & EPOLLIN) {
                Connection& connection = connectionMap.at(events[i].data.fd);
                processMessage(connection, events[i].data.fd, epollfd);
            }

            if (events[i].events & EPOLLOUT) {
                auto it = connectionMap.find(events[i].data.fd);
                if (it != connectionMap.end() && it->second.processOutgoingMessage() != -1) {
                    epoll_event clientEvent{};
                    clientEvent.events = EPOLLIN; 
                    clientEvent.data.fd = events[i].data.fd;
                    epoll_ctl(epollfd, EPOLL_CTL_MOD, events[i].data.fd, &clientEvent);
                }
            }
        }
    }

    close(sockfd); 
    return 0;
} 