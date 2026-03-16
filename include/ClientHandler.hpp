#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <winsock2.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <set>
#include "../include/GlobalDebug.hpp"

// Funzione principale che resta in ascolto di nuovi client
void listener_thread(std::atomic<bool>& exit, SOCKET& listenSocket);

#endif