#define WIN32_LEAN_AND_MEAN

#include "ReadData.h"
#include "DataModel.hpp"
#include "ClientHandler.hpp"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#define MAX_TITLE_LENGTH 64

#pragma comment(lib, "ws2_32.lib")

void setForeground(const char* win_title) {
	HWND my_hwnd = GetConsoleWindow();
	HWND other_hwnd = NULL;

	// Finding the other instance
	while ((other_hwnd = FindWindowExA(NULL, other_hwnd, NULL, win_title)) != NULL) {
		if (other_hwnd != my_hwnd) {
			ShowWindow(other_hwnd, SW_RESTORE);
			SetForegroundWindow(other_hwnd);
			break; 
		}
	}
}


int main() {

	SetConsoleTitleA("ACC UDP SERVER");

	std::cout << "[MAIN] Thread " << std::this_thread::get_id() << " iniziato." << std::endl;

	// Instruction to exit the program
	std::cout << "Press the right control key to exit the program." << std::endl;

	const char* mutexName = "Global\\ACC_GT3_Telemetry_Mutex";
	HANDLE hMutex = CreateMutexA(NULL, FALSE, mutexName);

	if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {

		// Anothr instance is opened
		MessageBoxA(
			NULL,
			"Attenzione un'altra istanza è già attiva.\n\n"
			"Chiudi l'altra finestra prima di avviarne una nuova.",
			"Errore Avvio - Telemetria ACC",
			MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST
		);
		char title[MAX_TITLE_LENGTH];
		if (GetConsoleTitleA(title, MAX_TITLE_LENGTH) == 0) {
			return 1;
		}
		setForeground(title);
		if (hMutex) CloseHandle(hMutex);
		return 1;
	}

	// Multi thread server architecture

	// Atomic variabile for the loop
	std::atomic<bool> exit{ false };

	// Atomic variable to check if shared memory is initialized
	std::atomic<bool> initialized{ false };

	

	// Thread to listen for "down arrow" key press to exit the loop
	std::thread readInput([&exit, &initialized]() {

		std::cout << "[INPUT] Thread per la lettura di " << std::this_thread::get_id() << " iniziato." << std::endl;
		while (!exit) {
			if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) {
				exit = true;
				initialized = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		std::cout << "Exit key pressed, shutting down..." << std::endl;

	});

	SPageFileGraphic graphicsData;
	SPageFilePhysics physicsData;
	SPageFileStatic staticData;

	if (!initialized) {
		std::cout << "Waiting for shared memory to be initialized...\nJoin a session to start." << std::endl;
	}
	while (!initialized) {
		if (InitSM() == 1) {
			initialized = true;
			std::cout << "Shared memory initialized successfully." << std::endl;
		}
		else {
			Sleep(1000);
		}
	}
	// Now SM is initialized

	Packet payload;

	// --- 1. SETUP RETE (WINSOCK) --- 
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}

	// Creazione del Socket UDP
	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = htons(9999); // Port where the client has to connect to
	if (bind(serverSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
		std::cout << "[ERROR] Bind fallito: " << WSAGetLastError() << std::endl;
		// Se fallisce, puliamo e usciamo, inutile lanciare i thread
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	// Faccio partire il thread che ascolta
	std::thread s_listener(listener_thread, std::ref(exit), std::ref(serverSocket));

	// Thread to update the data model
	std::thread sm_reader([&exit]() {
		std::cout << "[S.M.] Thread " << std::this_thread::get_id() << " iniziato." << std::endl;
		while (!exit) {
			SPageFileGraphic g;
			SPageFilePhysics p;
			SPageFileStatic s;
			
			ReadPhysics(&p);
			ReadGraphics(&g);
			ReadStatic(&s);

			DataModel::getInstance().updateData(g, p, s);

			Sleep(16); // Sleep for 16ms to achieve ~60Hz update rate
		}

		std::cout << "Shared memory reader thread exiting..." << std::endl;
	});

	// Active clients set
	std::set<ClientAddress> activeClients;

	// This thread sends data
	while (!exit) {
		payload = DataModel::getInstance().getPacket();

		// Blocchiamo la lista client solo il tempo necessario per inviare
		activeClients = DataModel::getInstance().getClients();
		for (const auto& client : activeClients) {
			int res = sendto(serverSocket, (const char*)&payload, sizeof(payload), 0,
				(sockaddr*)&client.addr, sizeof(client.addr));

			if (res == SOCKET_ERROR) {
				int err = WSAGetLastError();
				// Se vedi l'errore 10054 qui, significa che il client Python ha chiuso 
				// o il firewall sta bloccando.
				std::cout << "[SEND ERROR] Client irraggiungibile, errore: " << err << std::endl;
			}
			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// 3. Ora che il socket è chiuso, il listener si sveglia, vede exit=true e finisce
	readInput.join();
	closesocket(serverSocket);
	if (s_listener.joinable()) s_listener.join();
	if (sm_reader.joinable()) sm_reader.join();

	WSACleanup();
	DismissSM();


	std::cout << "Telemetry stream stopped." << std::endl;
	return 0;


}

