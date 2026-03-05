#define WIN32_LEAN_AND_MEAN

#include "ReadData.h"
#include "DataModel.hpp"
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

	// --- 1. SETUP RETE (WINSOCK) --- 
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return 1;
	}

	// Creazione del Socket UDP
	SOCKET sendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sendSocket == INVALID_SOCKET) {
		std::cout << "Error creating socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	bool broadcastEnable = true;

	int ret = setsockopt(sendSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcastEnable, sizeof(broadcastEnable));
	// Definizione destinazione (Localhost:9999)
	sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(9999); // Port where the client has to connect to
	inet_pton(AF_INET, "255.255.255.255", &destAddr.sin_addr); // Broadcast address to reach all clients in the local network MODIFY to UNICAST

	// Thread to listen for "down arrow" key press to exit the loop
	std::thread readInput([&sendSocket, &exit, &initialized]() {

		while (!exit) {
			if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) {
				exit = true;
				initialized = true;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

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

	// Thread to update the data model
	std::thread sm_reader([&exit]() {
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
		});

	while (!exit) {

		payload = DataModel::getInstance().getPacket();

		int bytesSent = sendto(sendSocket, (const char*)&payload, sizeof(payload), 0, (sockaddr*)&destAddr, sizeof(destAddr));

		if (bytesSent == SOCKET_ERROR) {
			std::cout << "Send failed: " << WSAGetLastError() << std::endl;
		}

		Sleep(16);

	}

	readInput.join();
	sm_reader.join();

	WSACleanup();
	DismissSM();
	closesocket(sendSocket);

	std::cout << "Telemetry stream stopped." << std::endl;
	return 0;


}

