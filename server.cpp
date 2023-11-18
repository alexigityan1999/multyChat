#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <thread> 
#include <mutex> 
#include <algorithm>
#include <functional>
#include <string>

using namespace std;

mutex clientsMutex;

vector<thread> clientThreads;

// Thread function to handle communication with a client
int ClientThread(SOCKET client, vector<SOCKET>& clients) {
    char buffer[4096];
    int bytesRead;

    while (true) {
        bytesRead = recv(client, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAECONNRESET || error == WSAENOTCONN) {
                // Connection is reset or client is not connected
                cout << "Client disconnected or not connected." << endl;
            } else {
                cout << "Error receiving data from client: " << error << endl;
            }
            break;
        }

        if (bytesRead == 0) {
            // Connection is closed by the client
            cout << "Client closed connection." << endl;
            break;
        }

        // Broadcast the received message to all clients
        clientsMutex.lock();
        for (auto it = clients.begin(); it != clients.end();) {
            SOCKET currClient = *it;

            int sendResult = send(currClient, buffer, bytesRead, 0);

            if (sendResult == SOCKET_ERROR) {
                int error = WSAGetLastError();
                if (error == WSAECONNRESET || error == WSAENOTCONN) {
                    // Connection is reset or client is not connected
                    cout << "Client disconnected or not connected." << endl;
                } else {
                    cout << "Failed to send message to a client. Error code: " << error << endl;
                }

                // Remove the client from the vector and close the socket
                it = clients.erase(it);
                closesocket(currClient);
            } else {
                // Move to the next client
                ++it;
            }
        }
        clientsMutex.unlock();
    }

    // Remove the current client from the vector
    clientsMutex.lock();
    auto it = find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
    }
    clientsMutex.unlock();

    // Close the socket for the current client
    if (closesocket(client) == SOCKET_ERROR) {
        cout << "Error closing client socket" << endl;
    }

    return 0;
}

int main() {
    // Server configuration
    const char* SERVER_ADDRESS = "127.0.0.1";
    const int SERVER_PORT = 1234;   

    // Initialize Winsock
    WSADATA wsData;
    WORD version = MAKEWORD(2, 2);

    int winsockInit = WSAStartup(version, &wsData);
    if (winsockInit != 0) {
        cout << "Winsock library initialization failed" << endl;
        return 1;
    }

    // Create the server socket
    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        cout << "Failed to create server socket" << endl;
        return 1;
    }

    // Server address configuration
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &(hint.sin_addr));

    // Bind the server socket
    if (bind(server, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
        cout << "Failed to bind the server socket" << endl;
        closesocket(server);
        WSACleanup();
        return 1;
    }

    // Start listening for incoming connections
    listen(server, SOMAXCONN);

    // Vector to store connected clients
    vector<SOCKET> clients;

    bool running = true;

    while (running) {
        // Accept a new client connection
        SOCKET client = accept(server, nullptr, nullptr);
        clientsMutex.lock();
        clients.push_back(client);
        clientsMutex.unlock();

        if (client == INVALID_SOCKET) {
            cout << "Error accepting client connection" << endl;
            continue;
        }

        // Create a new thread to handle the client
        clientThreads.push_back(thread(ClientThread, client, ref(clients)));
    }
    
    // Wait for all client threads to finish
    for (auto& thread : clientThreads) {
        thread.join();
    }

    // Close the server socket and clean up Winsock
    closesocket(server);
    WSACleanup();

    return 0;
}
