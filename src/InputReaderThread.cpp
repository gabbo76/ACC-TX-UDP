#include "../include/InputReaderThread.hpp"

void readInputThread(std::atomic<bool>& exit){

    while (!exit) {
        if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) {

            std::cout << "\n[INPUT] Manual mode, insert a IP address" << std::endl;

            std::string newIp;
            if (std::cin >> newIp) {

                sockaddr_in sa;
                ZeroMemory(&sa, sizeof(sa));
                sa.sin_family = AF_INET;
                sa.sin_port = htons(9999); // Client must connect to this port

                // String to address
                int result = inet_pton(AF_INET, newIp.c_str(), &(sa.sin_addr));

                if (result == 1) {
                    DataModel::getInstance().addClient(sa);
                }
                else {
                    std::cout << "[ERRORE] IP not valid: " << newIp << std::endl;
                }
            }
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
