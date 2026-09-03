#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <thread>

using namespace std;

struct addrinfo hints; // struct that contains information of the connection
struct addrinfo *serverInfo; // pointer to the results

// runs on its own thread so recv() can block independently of the send loop in main()
void receiveLoop(int sockfd) {
    uint8_t buf[4096];
    while (true) {
        ssize_t bytes_read = recv(sockfd, buf, sizeof(buf), 0);
        if (bytes_read == 0) {
            cout << "Server closed the connection." << '\n';
            break;
        }
        if (bytes_read == -1) {
            cout << "Read error: " << strerror(errno) << '\n';
            break;
        }
        cout << "Received " << bytes_read << " bytes from server: " << string(reinterpret_cast<char*>(buf), bytes_read) << '\n';
    }
}


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
        cout << "Failed to connect to server because " << strerror(errno);
        return errno;
    }
    cout << "Connected to server!" << '\n';

    // START THE RECEIVER BEFORE SENDING MESSAGE
    thread receiver(receiveLoop, sockfd);
    

    //SET "hello":"world" -> *3\r\n$3\r\nSET\r\n$5\r\nhello\r\n$5\r\nworld\r\n
    //GET "hello" -> *2\r\n$3\r\nGET\r\n$5\r\nhello\r\n
    std::string message = "*3\r\n$3\r\nSET\r\n$5\r\nhello\r\n$5\r\nworld\r\n*2\r\n$3\r\nGET\r\n$5\r\nhello\r\n*2\r\n$3\r\nGET\r\n$5\r\nhello\r\n*2\r\n$3\r\nGET\r\n$5\r\nhello\r\n*3\r\n$3\r\nSET\r\n$5\r\nbello\r\n$5\r\nzorld\r\n*2\r\n$3\r\nGET\r\n$5\r\nbello\r\n";

    size_t offset = 0;
    while (offset < message.length()) {
        ssize_t bytes_sent = send(sockfd, message.data() + offset, message.length() - offset, 0);
        if (bytes_sent == -1) {
            cout << "Send error: " << strerror(errno) << '\n';
            return errno;
        }
        cout << "Sent " << bytes_sent << " bytes to the server" << '\n';
        offset += bytes_sent;
    }

    receiver.join();

    return 0;
}