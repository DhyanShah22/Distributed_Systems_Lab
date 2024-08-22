#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define LOG_FILE "chat_log.txt"

std::mutex clientMutex;
std::unordered_map<int, std::string> clients;
std::mutex logMutex;

void logMessage(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    } else {
        std::cerr << "Unable to open log file" << std::endl;
    }
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::string message(buffer);

        if (message.find("/file") == 0) {
            std::string directory = "uploads/";
            std::string fileName = directory + message.substr(6);
            FILE* file = fopen(fileName.c_str(), "wb");
            if (file) {
                while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
                    fwrite(buffer, sizeof(char), bytesRead, file);
                    if (bytesRead < BUFFER_SIZE) break;
                }
                fclose(file);
                std::cout << "File received: " << fileName << std::endl;
                logMessage("File received: " + fileName);
            } else {
                std::cerr << "Failed to open file for writing" << std::endl;
            }
        } else {
            std::lock_guard<std::mutex> guard(clientMutex);
            for (const auto& [socket, name] : clients) {
                if (socket != clientSocket) {
                    send(socket, buffer, bytesRead, 0);
                }
            }
            logMessage("Client " + std::to_string(clientSocket) + ": " + message);
        }
    }

    close(clientSocket);
    std::lock_guard<std::mutex> guard(clientMutex);
    clients.erase(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    std::ofstream logFile(LOG_FILE, std::ios::trunc);
    logFile.close();

    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("Accept failed");
            continue;
        }

        std::string welcomeMessage = "Welcome to the chat server!";
        send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length(), 0);

        std::lock_guard<std::mutex> guard(clientMutex);
        clients[clientSocket] = "Client";

        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
    return 0;
}
