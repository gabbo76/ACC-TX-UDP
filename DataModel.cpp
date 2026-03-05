#include "DataModel.hpp"

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

    return packet;
}