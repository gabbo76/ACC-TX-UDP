#include "ClientHandler.hpp"
#include "DataModel.hpp"
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>

void listener_thread(std::atomic<bool>& exit, SOCKET listenSocket) {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string threadIdStr = ss.str();
    LogToFile("[Listener] Thread " + threadIdStr + ".");
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[64];

    while (!exit) {
        int bytes = recvfrom(listenSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &addrLen);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            if (strncmp(buffer, "START", 5) == 0) {
                std::cout << "[REGISTRY] Nuovo client registrato!" << std::endl;
                DataModel::getInstance().addClient(clientAddr);
            }
        }else if (bytes == 0) {
            // UDP è connectionless, ma su alcuni sistemi bytes == 0 può indicare shutdown
            std::cout << "[Listener] Ricevuto pacchetto vuoto o shutdown." << std::endl;
        }else {
            int error = WSAGetLastError();

            // Se exit è true, l'errore è normale (abbiamo chiuso il socket dal main)
            if (exit) {
                std::cout << "[Listener] Chiusura programmata sbloccata." << std::endl;
                break;
            }

            // Se arrivi qui, c'è un problema vero
            std::cerr << "[CRITICAL] Errore recvfrom: " << error << std::endl;

            // Se l'errore è 10004 (WSAEINTR), è un'interruzione esterna
            // Se l'errore è 10038, il socket non è più un socket valido!
            if (error == WSAENOTSOCK || error == WSAECONNRESET) {
                break; // Esci dal loop perché il socket è andato
            }
        }
    }

    std::cout << "[Listener] Thread in uscita..." << std::endl;
}
