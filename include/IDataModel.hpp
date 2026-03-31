#ifndef IDATAMODEL_HPP
#define IDATAMODEL_HPP
#include <mutex>
#include <set>
#include <shared_mutex>
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <iostream>

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

    int status = 0;
    int session = 0;
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

    int flag = 0;
    int fuelXLap = 0;

};

typedef struct _packet Packet;

class IDataModel {

public:

    virtual IDataModel& operator=(const IDataModel&) = delete;
    virtual ~IDataModel() = default;

    // Update data
    virtual void updateData(void* data) = 0;

    // Get packet
    virtual Packet getPacket() = 0;

    void addClient(const sockaddr_in& clientAddr) {
        std::lock_guard<std::mutex> lock(_clientsMutex);

        auto result = _activeClients.insert({ clientAddr });

        if (result.second) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
            std::cout << "[DATAMODEL] Nuovo client registrato: " << ip
                << ":" << ntohs(clientAddr.sin_port) << std::endl;
        }
    }

    void removeClient(const sockaddr_in& clientAddr) {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        std::cout << "[DATAMODEL] Client rimosso" << std::endl;
        _activeClients.erase({ clientAddr });
    }

    std::set<ClientAddress> getClients() {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        return _activeClients;
    }

protected: 
    IDataModel() {}

    // Mutex for writing/reading the data
    static std::shared_mutex _dataMutex;

    // Mutex for writing/reading the clients list
    static std::mutex _clientsMutex;

    // Active clients
    std::set<ClientAddress> _activeClients;

    // Mutex for the instance
    static std::mutex _instanceMutex;

};

#endif