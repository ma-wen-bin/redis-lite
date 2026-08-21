#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <cstddef>
#include "util/ring_buffer.h"
#include "util/parser.h"

using namespace std;

int backlog {10}; //number of connections allowed
struct addrinfo hints; // struct that contains information of the connection 
struct addrinfo *servInfo; // pointer to the results
struct sockaddr_storage clientAddr;
int newSockfd; //new socket file descriptor (client's)
RingBuffer buffer(8192); //ring buffer
MessageParser parser;

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
    
    //RECEIVE INCOMING MESSAGES
    while (true) {
        if (buffer.isFull()) {
            //peek to view all available data
            const auto& [ readPtr, readLen ] = buffer.peek();
            size_t bytes_consumed = parser.consumeBytes(readPtr, readLen);
            buffer.consume(bytes_consumed);
            
            if (bytes_consumed == 0) { // no delimiter found -> close connection 
                cout << "Message exceeds buffer size, closing connection." << '\n';
                exit(EXIT_FAILURE);
            }

        } else {
            cout << "Current buffer size: " << buffer.size() << '\n';
            const auto& [ writePtr, writeLen ] = buffer.writeableSpan(); // get the starting point and length that is empty
            size_t bytes_read = recv(newSockfd, writePtr, writeLen, 0); // write to the buffer 
            cout << "Read :" << bytes_read << " bytes" << '\n';
            buffer.commitWrite(bytes_read); // update the head pointer's position 

            if (bytes_read == -1) {
                cout << "Read error : " << strerror(errno) << '\n';
                break;
            }

            if (bytes_read == 0) {
                cout << "Client disconnected." << '\n';
                break;
            } 

            const auto& [ readPtr, readLen ] = buffer.peek();
            size_t bytes_consumed = parser.consumeBytes(readPtr, readLen);
            if (bytes_consumed > 0) {
                buffer.consume(bytes_consumed);
                cout << "Parser has consumed: " << bytes_consumed << '\n';
            }

        }  
    }

    return 0;
} 