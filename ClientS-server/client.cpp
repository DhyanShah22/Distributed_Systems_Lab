#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 1024

void receiveMessages(int sockfd) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }
}

void sendFile(int sockfd, const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (file) {
        std::string command = "/file " + fileName;
        send(sockfd, command.c_str(), command.length(), 0);

        char buffer[BUFFER_SIZE];
        while (file.read(buffer, sizeof(buffer))) {
            send(sockfd, buffer, file.gcount(), 0);
        }
        send(sockfd, buffer, file.gcount(), 0);
        file.close();
    } else {
        std::cerr << "File not found" << std::endl;
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    std::string serverIP = "127.0.0.1";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    std::thread recvThread(receiveMessages, sockfd);

    while (true) {
        std::string message;
        std::getline(std::cin, message);

        if (message == "/file") {
            std::string fileName;
            std::cout << "Enter file name to send: ";
            std::getline(std::cin, fileName);
            message = "/file " + fileName;
            sendFile(sockfd, fileName);
        } else {
            send(sockfd, message.c_str(), message.length(), 0);
        }
    }

    close(sockfd);
    return 0;
}
