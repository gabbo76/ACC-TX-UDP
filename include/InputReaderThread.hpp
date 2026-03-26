#ifndef INPUT_READER_THREAD_HPP
#define INPUT_READER_THREAD_HPP

#define WIN32_LEAN_AND_MEAN

#include <atomic>
#include <sstream>
#include <iostream>
#include "../include/ReadData.h"
#include <winsock2.h>
#include "../include/DataFactory.hpp"
#include <WS2tcpip.h>
#include "../include/GlobalDebug.hpp"
#include "../include/Config.hpp"

#pragma comment(lib, "ws2_32.lib")

/*
* @brief Thread to read the user input and add manually new IPs to the clients list
* 
* @param exit Reference to the atomic boolean variable that signals when the thread should exit
*/
void readInputThread(std::atomic<bool>& exit);

#endif