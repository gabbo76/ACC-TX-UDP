#define WIN32_LEAN_AND_MEAN

#include "ReadData.h"
#include "DataModel.hpp"
#include "ClientHandler.hpp"
#include "ThreadManager.hpp"
#include "GlobalDebug.hpp"
#include "InputReaderThread.hpp"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <windows.h>
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

	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string threadIdStr = ss.str();
	LogToFile("[Main] Thread " + threadIdStr + ".");

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
		std::cout << "Waiting for shared memory to be initialized...\nJoin a session to start." << std::endl;
	}
	while (!initialized) {
		if (InitSM() == 1) {
			initialized = true;
			std::cout << "Shared memory initialized successfully." << std::endl;
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
	localAddr.sin_port = htons(9999); // Port where the client has to connect to
	if (bind(serverSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
		std::cout << "[ERROR] Bind fallito: " << WSAGetLastError() << std::endl;
		// If something went wrong, cleanup and exit
		closesocket(serverSocket);
		WSACleanup();
		return 1;
	}


	// Starting the threads if the S.M. is initialized correctly
	if (initialized) {
		// Thread to listen for new clients
		std::thread s_listener(listener_thread, std::ref(exit), std::ref(serverSocket));
		ThreadManager::getInstance().addThread(std::move(s_listener));

		// Thread to update the data model
		std::thread sm_reader([&exit]() {
			std::stringstream ss;
			ss << std::this_thread::get_id();
			std::string threadIdStr = ss.str();
			LogToFile("[S.M. Reader] Thread " + threadIdStr + ".");
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

			std::cout << "[Shared Memory] Thread in uscita." << std::endl;
			});
		ThreadManager::getInstance().addThread(std::move(sm_reader));
	}


	// Active clients set
	std::set<ClientAddress> activeClients;

	// This thread sends data
	while (!exit) {
		payload = DataModel::getInstance().getPacket();

		activeClients = DataModel::getInstance().getClients();
		for (const auto& client : activeClients) {
			int res = sendto(serverSocket, (const char*)&payload, sizeof(payload), 0,
				(sockaddr*)&client.addr, sizeof(client.addr));

			if (res == SOCKET_ERROR) {
				int err = WSAGetLastError();
				std::cout << "[SEND ERROR] Client irraggiungibile, errore: " << err << std::endl;
			}
			
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	// Exit phase
	if (serverSocket != INVALID_SOCKET) {
		closesocket(getServerSocket());
	}
	std::vector<std::thread>& threads = ThreadManager::getInstance().getRegistry();
	// Wait for all threads to finish
	for (auto& thread : threads) {
		if (thread.joinable())  {
			thread.join();
		}
	}
	DismissSM();
	WSACleanup();
	SetEvent(readyToExit);
	CloseHandle(readyToExit);
	return 0;

}

