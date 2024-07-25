#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

const int PORT = 8080;

void handleClient(int clientSocket){
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cerr << "Client disconnected" << endl;
            close(clientSocket);
            break;
        }
        cout << "Client: " << buffer << endl;
        string response;
        cout << "Server: ";
        getline(cin, response);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Failed to create socket" << endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Failed to bind socket" << endl;
        close(serverSocket);
        return -1;
    }

    if (listen(serverSocket, 1) == -1) {
        cerr << "Failed to listen on socket" << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Server listening on port " << PORT << endl;

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        cerr << "Failed to accept client connection" << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Client connected" << endl;

    thread clientThread(handleClient, clientSocket);
    clientThread.join();

    close(serverSocket);
    return 0;
}