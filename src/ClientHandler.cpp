#include "../include/ClientHandler.hpp"
//#include "../include/DataModel.hpp"
#include "../include/DataFactory.hpp"
#include "../include/IDataModel.hpp"
#include "../include/Config.hpp"
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>

void listener_thread(std::atomic<bool>& exit, SOCKET& listenSocket) {

    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);
    char buffer[64];

    std::unique_ptr<DataFactory> sim_factory = DataFactory::getFactory(ConfigManager::getInstance().get().simType);
    IDataModel& dataModel = sim_factory->getModel();

    while (!exit) {
        int bytes = recvfrom(listenSocket, buffer, sizeof(buffer) - 1, 0, (sockaddr*)&clientAddr, &addrLen);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            if (strncmp(buffer, "START", 5) == 0) {
                dataModel.addClient(clientAddr);
            }else if(strncmp(buffer, "STOP", 4) == 0) {
                dataModel.removeClient(clientAddr);
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
            std::cerr << "[Listener] Error in recvfrom: " << error << std::endl;

            if (error == WSAENOTSOCK) {
                break; // Esci dal loop perché il socket è andato
            }
            else if (error == WSAECONNRESET) {
                // Questo errore può accadere se un client invia un pacchetto a un socket chiuso
				std::cout << "[Listener] Ricevuto pacchetto da client disconnesso." << std::endl;
			    dataModel.removeClient(clientAddr);
            }
        }
    }

    std::cout << "[Listener] Thread in uscita..." << std::endl;
}
