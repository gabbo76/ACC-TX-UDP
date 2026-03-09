#include <atomic>
#include <sstream>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "DataModel.hpp"
#include "GlobalDebug.hpp"

#pragma comment(lib, "ws2_32.lib")

void readInputThread(std::atomic<bool>& exit);