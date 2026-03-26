#pragma once

#include "../../include/IDataModel.hpp"
#include "../include/SharedFileOut.h"


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
    std::set<ClientAddress> _activeClients;


};

