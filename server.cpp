#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const std::string IP = "127.0.0.1";

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSA failed\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serv{};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr(IP.c_str());

    if (connect(sock, (sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR) {
        std::cerr << "Connect failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server\n";

    std::string msg;
    while (std::getline(std::cin, msg)) {
        if (msg.empty()) continue;
        send(sock, msg.c_str(), msg.size(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
// $Env:PATH += ";C:\w64devkit\bin"
