#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <winsock2.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <set>

// Funzione principale che resta in ascolto di nuovi client
void listener_thread(std::atomic<bool>& exit, SOCKET listenSocket);

// Funzione dedicata al singolo client (lanciata in un thread separato)
void client_handler(sockaddr_in clientAddr, std::atomic<bool>& exit, SOCKET s);

#endif