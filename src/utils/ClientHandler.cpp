#include "../include/ClientHandler.hpp"
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

            // Adding new client
            if (strncmp(buffer, "START", 5) == 0) {
                dataModel.addClient(clientAddr);

            //Removing a client
            }else if(strncmp(buffer, "STOP", 4) == 0) {
                dataModel.removeClient(clientAddr);
            }

            // Updating last seen for alive client
            else if (strncmp(buffer, "ALIVE", 5) == 0) {
                dataModel.updateLastSeenClient(clientAddr);
            }
        }else if (bytes == 0) {
            // UDP connectionless
            std::cout << "[Listener] Received empty packet." << std::endl;
        }else {
            int error = WSAGetLastError();

            if (exit) {
                break;
            }

            // ERROR
            std::cerr << "[Listener] Error in recvfrom: " << error << std::endl;

            if (error == WSAENOTSOCK) {
                break;
            }
            else if (error == WSAECONNRESET) {
			    dataModel.removeClient(clientAddr);
            }
        }
    }

    std::cout << "[Listener] Thread in uscita..." << std::endl;
}
