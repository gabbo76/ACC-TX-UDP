#pragma once
#include "SharedFileOut.h"
#include "ReadData.h"
#include <mutex>
#include <set>
#include <shared_mutex>
#include <WinSock2.h>

struct ClientAddress {
    sockaddr_in addr;

    bool operator<(const ClientAddress& other) const {
        if (addr.sin_addr.s_addr != other.addr.sin_addr.s_addr)
            return addr.sin_addr.s_addr < other.addr.sin_addr.s_addr;
        return addr.sin_port < other.addr.sin_port;
    }
};

// Struct of the pachet returned to the client, containing all the data we want to send
struct _packet {
    float gas = 0;
    float brake = 0;
    float fuel = 0;
    int gear = 0;
    int rpm = 0;
    float steerAngle = 0;
    float speedKmh = 0;
    float accG[3]{};
    float wheelSlip[4]{};
    float wheelLoad[4]{};
    float wheelsPressure[4]{};
    float wheelAngularSpeed[4]{};
    float tyreWear[4]{};
    float tyreDirtyLevel[4]{};
    float tyreCoreTemperature[4]{};
    float camberRAD[4]{};
    float suspensionTravel[4]{};

    float tyreTempI[4]{};
    float tyreTempM[4]{};
    float tyreTempO[4]{};

    float brakeBias = 0;
    int tcinAction = 0;
    int absInAction = 0;
    float suspensionDamage[4]{};
    float tyreTemp[4]{};
    float waterTemp = 0;
    float brakePressure[4]{};
    int frontBrakeCompound = 0;
    int rearBrakeCompound = 0;
    float padLife[4]{};
    float discLife[4]{};
    int ignitionOn = 0;
    int starterEngineOn = 0;
    int isEngineRunning = 0;

    AC_STATUS status = AC_OFF;
    AC_SESSION_TYPE session = AC_PRACTICE;
    wchar_t currentTime[15]{};
    wchar_t lastTime[15]{};
    wchar_t bestTime[15]{};
    wchar_t split[15]{};
    int completedLaps = 0;
    int position = 0;
    int iCurrentTime = 0;
    int iLastTime = 0;
    int iBestTime = 0;
    float sessionTimeLeft = 0;

    AC_FLAG_TYPE flag = AC_NO_FLAG;
    int fuelXLap = 0;

};

typedef struct _packet Packet;


class DataModel {
public:
	static DataModel& getInstance();
	DataModel(const DataModel&) = delete;
	void operator=(const DataModel&) = delete;

	// Update data
	void updateData(SPageFileGraphic& g, SPageFilePhysics& p, SPageFileStatic& s);

	// Get packet
	Packet getPacket();

    // Add client to the list
    void addClient(const sockaddr_in& clientAddr);

    // Remove a client from the list
    void removeClient(const sockaddr_in& clientAddr);

	//Returns a set with all the clients currently connected
    std::set<ClientAddress> getClients();

private:
	DataModel() {};


	// Mutex for writing/reading the data
	mutable std::shared_mutex _dataMutex;

    // Mutex for writing/reading the clients list
    mutable std::mutex _clientsMutex;

    // Private copy of the structs read from the shared memory used to create the packet
	SPageFileGraphic graphicsData;
	SPageFilePhysics physicsData;
	SPageFileStatic staticData;
	Packet packet;

    std::set<ClientAddress> _activeClients;
};