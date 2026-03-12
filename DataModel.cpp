#define WIN32_LEAN_AND_MEAN

#include "DataModel.hpp"
#include <ws2tcpip.h>

DataModel& DataModel::getInstance() {
    // Questa riga viene eseguita SOLO la prima volta che chiami il metodo
    static DataModel instance;
    return instance;
}

void DataModel::updateData(SPageFileGraphic& g, SPageFilePhysics& p, SPageFileStatic& s) {
	// Locking the mutex (only 1 can write at the same time)
    std::unique_lock<std::shared_mutex> lock(_dataMutex);
    graphicsData = g;
    physicsData = p;
	staticData = s;
}

Packet DataModel::getPacket() {
    // Locking the mutex (more than 1 can read at the same time)

    SPageFileGraphic g_snap;
    SPageFilePhysics p_snap;
    SPageFileStatic s_snap;

	// Making a snap of the data to minimize the time we hold the lock
    {
        std::shared_lock<std::shared_mutex> lock(_dataMutex);
        g_snap = graphicsData;
        p_snap = physicsData;
        s_snap = staticData;
    }

    Packet packet;

    packet.gas = p_snap.gas;
    packet.brake = p_snap.brake;
    packet.fuel = p_snap.fuel;
    packet.gear = p_snap.gear - 1;
    packet.rpm = p_snap.rpm;
    packet.steerAngle = p_snap.steerAngle;
    packet.speedKmh = p_snap.speedKmh;
          
    for (int i = 0; i < 3; i++) {
        packet.accG[i] = p_snap.accG[i];
    }
    for (int i = 0; i < 4; i++) {
        packet.wheelSlip[i] = p_snap.wheelSlip[i];
        packet.wheelLoad[i] = p_snap.wheelLoad[i];
        packet.wheelsPressure[i] = p_snap.wheelsPressure[i];
        packet.wheelAngularSpeed[i] = p_snap.wheelAngularSpeed[i];
        packet.tyreWear[i] = p_snap.tyreWear[i];
        packet.tyreDirtyLevel[i] = p_snap.tyreDirtyLevel[i];
        packet.tyreCoreTemperature[i] = p_snap.tyreCoreTemperature[i];
        packet.camberRAD[i] = p_snap.camberRAD[i];
        packet.suspensionTravel[i] = p_snap.suspensionTravel[i];
        packet.tyreTempI[i] = p_snap.tyreTempI[i];
        packet.tyreTempM[i] = p_snap.tyreTempM[i];
        packet.tyreTempO[i] = p_snap.tyreTempO[i];
        packet.suspensionDamage[i] = p_snap.suspensionDamage[i];
        packet.tyreTemp[i] = p_snap.tyreTemp[i];
        packet.brakePressure[i] = p_snap.brakePressure[i];
        packet.padLife[i] = p_snap.padLife[i];
        packet.discLife[i] = p_snap.discLife[i];
    }

    packet.brakeBias = p_snap.brakeBias;
    packet.tcinAction = p_snap.tcinAction;
    packet.absInAction = p_snap.absInAction;
    packet.waterTemp = p_snap.waterTemp;

    /*
    packet.frontBrakeCompound = p_snap.frontBrakeCompound;
    packet.rearBrakeCompound = p_snap.rearBrakeCompound;
    packet.ignitionOn = p_snap.ignitionOn;
    packet.starterEngineOn = p_snap.starterEngineOn;
    packet.isEngineRunning = p_snap.isEngineRunning;
    */

    /*
    --- Dati grafici (SPageFileGraphic) --- (Bug fix: prima non venivano copiati!)
    packet.status = g_snap.status;
    packet.session = g_snap.session;
    packet.completedLaps = g_snap.completedLaps;
    packet.position = g_snap.position;
    packet.iCurrentTime = g_snap.iCurrentTime;
    packet.iLastTime = g_snap.iLastTime;
    packet.iBestTime = g_snap.iBestTime;
    packet.sessionTimeLeft = g_snap.sessionTimeLeft;
    packet.flag = g_snap.flag;
    packet.fuelXLap = g_snap.fuelXLap;
    wcsncpy_s(packet.currentTime, g_snap.currentTime, 14);
    wcsncpy_s(packet.lastTime, g_snap.lastTime, 14);
    wcsncpy_s(packet.bestTime, g_snap.bestTime, 14);
    wcsncpy_s(packet.split, g_snap.split, 14)
    */
    return packet;
}

void DataModel::addClient(const sockaddr_in& clientAddr) {
    std::lock_guard<std::mutex> lock(_clientsMutex);

    auto result = _activeClients.insert({ clientAddr });

    if (result.second) {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ip, INET_ADDRSTRLEN);
        std::cout << "[DATAMODEL] Nuovo client registrato: " << ip
            << ":" << ntohs(clientAddr.sin_port) << std::endl;
    }
}

void DataModel::removeClient(const sockaddr_in& clientAddr) {
    std::lock_guard<std::mutex> lock(_clientsMutex);
    _activeClients.erase({ clientAddr });
}

std::set<ClientAddress> DataModel::getClients() {
    std::lock_guard<std::mutex> lock(_clientsMutex);
    return _activeClients;
}