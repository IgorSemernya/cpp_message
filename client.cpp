#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const int MAX_CLIENTS = 10;

std::vector<SOCKET> clients;

void broadcast(const std::string& message) {
    for (SOCKET client : clients) {
        send(client, message.c_str(), message.size(), 0);
    }
}

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;
        std::string msg(buffer, bytesReceived);
        std::cout << "Client: " << msg << std::endl;
        broadcast(msg + "\n");
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSA failed\n";
        return 1;
    }

    SOCKET serv = socket(AF_INET, SOCK_STREAM, 0);
    if (serv == INVALID_SOCKET) {
        std::cerr << "Socket failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serv, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(serv);
        WSACleanup();
        return 1;
    }

    if (listen(serv, MAX_CLIENTS) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(serv);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started on port " << PORT << "\n";

    while (true) {
        SOCKET client = accept(serv, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;
        clients.push_back(client);
        std::cout << "Client connected\n";
        std::thread(handleClient, client).detach();
    }

    closesocket(serv);
    WSACleanup();
    return 0;
}
