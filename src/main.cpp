#define WIN32_LEAN_AND_MEAN

#include "../include/ReadData.h"
#include "../include/DataModel.hpp"
#include "../include/ClientHandler.hpp"
#include "../include/ThreadManager.hpp"
#include "../include/GlobalDebug.hpp"
#include "../include/InputReaderThread.hpp"
#include "../include/SharedMemoryReaderThread.hpp"
#include "../include/Config.hpp"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <sstream>
#include <fstream>


#define MAX_TITLE_LENGTH 64

#pragma comment(lib, "ws2_32.lib")

HANDLE readyToExit;

std::atomic<bool>& getExitFlag() {
	static std::atomic<bool> exitFlag{ false };
	return exitFlag;
}

SOCKET& getServerSocket() {
	static SOCKET serverSocket = INVALID_SOCKET;
	return serverSocket;
}

// Handler to close gracefully the program when the user clicks the "X" button on the console window
BOOL WINAPI ConsoleHandler(DWORD ctrlType) {
	if (ctrlType == CTRL_CLOSE_EVENT) {
		auto& exit = getExitFlag();
		exit = true;

		// Wait for the main thread to signal that all threads have been joined and it's safe to exit
		WaitForSingleObject(readyToExit, INFINITE);
		Sleep(100);
		return TRUE;
	}
	return FALSE;
}

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

	// Exit handle
	readyToExit = CreateEventA(NULL, TRUE, FALSE, NULL);

	// Handler to close gracefully the program when the user clicks the "X" button on the console window
	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		return 1;
	}

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

	ConfigManager::getInstance().load();

	// Atomic variabile for the loop
	auto& exit = getExitFlag();

	// Atomic variable to check if shared memory is initialized
	std::atomic<bool> initialized{ false };

	// Thread to add manually new IPs
	std::thread readInput(readInputThread, std::ref(exit));
	ThreadManager::getInstance().addThread(std::move(readInput));

	SPageFileGraphic graphicsData;
	SPageFilePhysics physicsData;
	SPageFileStatic staticData;

	if (!initialized) {
		std::cout << "[START] Waiting for shared memory to be initialized...\nJoin a session to start." << std::endl;
	}
	while (!initialized) {
		if (InitSM() == 1) {
			initialized = true;
			std::cout << "[Shared Memory] Shared memory initialized successfully." << std::endl;
		}
		else if (getExitFlag()) {
			break;
		}
		else {
			Sleep(1000);
		}
	}

	Packet payload;

	// Setting up Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}

	// Creating UDP SOCKET
	SOCKET& serverSocket = getServerSocket();
	serverSocket =	socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = htons(ConfigManager::getInstance().get().serverPort); // Port where the client has to connect to
	if (bind(serverSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
		std::cout << "[ERROR] Bind failed: " << WSAGetLastError() << std::endl;
		// If something went wrong, cleanup and exit
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "[DEBUG] Server opened at port: " << ConfigManager::getInstance().get().serverPort << std::endl;


	// Starting the threads if the S.M. is initialized correctly
	if (initialized) {
		// Thread to listen for new clients
		std::thread s_listener(listener_thread, std::ref(exit), std::ref(serverSocket));
		ThreadManager::getInstance().addThread(std::move(s_listener));

		// Thread to update the data model
		std::thread sm_reader(readSharedMemoryThread, std::ref(exit));
		ThreadManager::getInstance().addThread(std::move(sm_reader));
	}


	// Active clients set
	std::set<ClientAddress> activeClients;

	int sleepMs = ConfigManager::getInstance().get().sleepMs();

	// This thread sends data
	while (!exit) {
		payload = DataModel::getInstance().getPacket();

		activeClients = DataModel::getInstance().getClients();
		for (const auto& client : activeClients) {
			int res = sendto(serverSocket, (const char*)&payload, sizeof(payload), 0, (sockaddr*)&client.addr, sizeof(client.addr));

			if (res == SOCKET_ERROR) {
				int err = WSAGetLastError();
				std::cout << "[SEND ERROR] Client irraggiungibile, errore: " << err << std::endl;
			}
			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
	}

	// Exit phase
	if (serverSocket != INVALID_SOCKET) {
		closesocket(getServerSocket());
	}

	// Wait for all threads to finish
	ThreadManager::getInstance().joinAll();

	DismissSM();
	WSACleanup();
	SetEvent(readyToExit);
	CloseHandle(readyToExit);
	return 0;

}

