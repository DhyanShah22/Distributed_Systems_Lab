#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

const int PORT = 8080;
const string SERVER_IP = "127.0.0.1";

void receiveMessages(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cerr << "Disconnected from server" << endl;
            close(clientSocket);
            break;
        }
        cout << "Server: " << buffer << endl;
    }
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Failed to create socket" << endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Failed to connect to server" << endl;
        close(clientSocket);
        return -1;
    }

    cout << "Connected to server" << endl;

    thread receiveThread(receiveMessages, clientSocket);

    string message;
    while (true) {
        cout << "Client: ";
        getline(cin, message);
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    receiveThread.join();
    close(clientSocket);
    return 0;
}
