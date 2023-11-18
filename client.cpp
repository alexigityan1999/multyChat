#include <iostream>
#include <WS2tcpip.h>
#include <chrono>
#include <thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

// Function to receive messages from the server
void receiveMessages(SOCKET clientSocket) {
    while (true) {
        char buffer[4096];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == SOCKET_ERROR) {
            int error = WSAGetLastError();
            cerr << "Failed to receive message from the server. Error code: " << error << endl;
            break;
        } else if (bytesRead == 0) {
            cout << "Server closed connection." << endl;
            break;
        }

        cout << buffer << endl;
    }
}

// Function to send messages to the server
void sendMessages(SOCKET clientSocket, const string& username) {
    while (true) {
        string userInput;
        cout << username << ": ";
        getline(cin, userInput);

        // Combine the username and message with a colon
        string message = username + ": " + userInput;

        int sendResult = send(clientSocket, message.c_str(), message.size() + 1, 0);
        if (sendResult == SOCKET_ERROR) {
            int error = WSAGetLastError();
            cerr << "Error sending data to the server. Error code: " << error << endl;
            break;
        }
    }
}

int main() {
    // Initialize Winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &wsData) != 0) {
        cerr << "Failed to initialize Winsock" << endl;
        return 1;
    }

    const char* SERVER_ADDRESS = "127.0.0.1";
    const int SERVER_PORT = 1234;

    // Create a socket for the client
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Failed to create client socket. Error code: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Get the username from the user
    cout << "Enter your username: ";
    string username;
    getline(cin, username);

    // Set up server address information
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &(hint.sin_addr));

    // Connect to the server
    if (connect(clientSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        cerr << "Failed to connect to the server" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Start two threads for receiving and sending messages
    thread receiveThread(receiveMessages, clientSocket);
    thread sendThread(sendMessages, clientSocket, username);

    // Wait for the threads to finish
    receiveThread.join();
    sendThread.join();

    // Clean up and close the socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
