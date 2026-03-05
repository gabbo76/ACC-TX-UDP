#include "ClientHandler.hpp"
#include "DataModel.hpp"
#include <ws2tcpip.h>
#include <iostream>

void client_handler(sockaddr_in clientAddr, std::atomic<bool>& exit, SOCKET s) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
    std::cout << "[THREAD] Gestione partita per: " << ip << std::endl;

    while (!exit) {
        
        Packet p = DataModel::getInstance().getPacket();
        sendto(s, (const char*)&p, sizeof(p), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "[THREAD] Chiusura per: " << ip << std::endl;
}

void listener_thread(std::atomic<bool>& exit, SOCKET listenSocket) {
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[64];

    while (!exit) {
        // Aspettiamo che un client ci "bussi" (es. manda "START")
        int bytes = recvfrom(listenSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &addrLen);

        if (bytes > 0 && buffer == "START") {
            std::thread([clientAddr, &exit, listenSocket]() {
                client_handler(clientAddr, exit, listenSocket);
            }).detach();
        }
    }
}