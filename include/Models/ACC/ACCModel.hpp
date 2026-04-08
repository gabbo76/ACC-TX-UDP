#pragma once

#include "../../IDataModel.hpp"
#include "../include/Models/ACC/SharedFileOut.h"


typedef struct DataRcv {
    SPageFileGraphic g;
    SPageFilePhysics p;
    SPageFileStatic s;
} DataRcv;

class ACCModel : public IDataModel {
public:
    static ACCModel& getInstance();

    void updateData(void* data);

    Packet getPacket();

protected:
    ACCModel() : IDataModel() {}

    SPageFileGraphic graphicsData{};
    SPageFilePhysics physicsData{};
    SPageFileStatic staticData{};

    // Active clients
    std::map<ClientAddress, Timestamp> _activeClients;


};

