#include <atomic>
#include <sstream>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "DataModel.hpp"
#include "GlobalDebug.hpp"

#pragma comment(lib, "ws2_32.lib")

/*
* @brief Thread to read the user input and add manually new IPs to the clients list
* 
* @param exit Reference to the atomic boolean variable that signals when the thread should exit
*/
void readInputThread(std::atomic<bool>& exit);