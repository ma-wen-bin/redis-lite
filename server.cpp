#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

using namespace std;

int backlog {10}; //number of connections allowed
struct addrinfo hints; // struct that contains information of the connection 
struct addrinfo *servInfo; // pointer to the results
struct sockaddr_storage clientAddr;
int newSockfd; //new socket file descriptor (client's)
uint8_t buffer[8192]; // buffer to store incoming messages

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
    int bytes_read; 
    int totalBytes = 0;

    while (totalBytes < sizeof(buffer) - 1) {
        bytes_read = recv(newSockfd, buffer + totalBytes, sizeof(buffer) - 1 - totalBytes, 0);
        if (bytes_read == -1) {
            cout << "Read error" << errno << endl;
            return errno;
        }

        if (bytes_read == 0) {
            cout << "Client disconnected." << endl;
            break;
        }

        totalBytes += bytes_read;
        if (buffer[totalBytes - 1] == '\n') {
            cout << "Found the end of message" << '\n';
            break;
        }

    }
    
    buffer[totalBytes] = '\0';
    string receivedText = reinterpret_cast<char*>(buffer);
    cout << "Received message: " << receivedText << '\n';


    return 0;
} 