#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

using namespace std;

int backlog {10}; //number of connections allowed
struct addrinfo hints; // struct that contains information of the connection 
struct addrinfo *serverInfo; // pointer to the results
struct sockaddr_storage clientAddr;
int newSockfd; //new socket file descriptor (client's)


int main() {
    
    //SET UP SERVER ADDRESS INFORMATION 
    memset(&hints, 0, sizeof(hints)); //ensure no garbage values 
    hints.ai_family = AF_INET; //only ipv4
    hints.ai_socktype = SOCK_STREAM;  //tcp stream sockets 

    int status = getaddrinfo("127.0.0.1", "6380", &hints, &serverInfo); 
    if (status != 0) { 
        cout << gai_strerror(status);
        exit(EXIT_FAILURE);
    }

    //CREATE THE CLIENT SOCKET
    int sockfd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

    //CONNECT TO THE SERVER 
    int connectionStatus = connect(sockfd, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if (connectionStatus == -1) {
        cout << "Failed to connect to server because " << std::strerror(errno);
        return errno;
    }
    cout << "Connected to server!" << '\n';

    std::string message = "Hello from client \n";
    
    int bytes_sent = 0;
    while (bytes_sent < message.length()) {
        bytes_sent = send(sockfd, message.data(), message.length(), 0);
        bytes_sent += bytes_sent; 
        cout << "Sent " << bytes_sent << " bytes to the server" << '\n';
        if (bytes_sent == -1) {
            return errno;
        }
    }

    return 0;
} 