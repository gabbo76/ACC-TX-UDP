#include "PacketMaker.h"

void MakePacket(SPageFilePhysics* physicsData, SPageFileGraphic* graphicsData, SPageFileStatic* staticData,Packet* packet) {
	packet->gas = physicsData->gas;
	packet->brake = physicsData->brake;
	packet->fuel = physicsData->fuel;
	packet->gear = physicsData->gear -1;
	packet->rpm = physicsData->rpm;
	packet->steerAngle = physicsData->steerAngle;
	packet->speedKmh = physicsData->speedKmh;

	for (int i = 0; i < 3; i++) {
		packet->accG[i] = physicsData->accG[i];
	}
	for (int i = 0; i < 4; i++) {
		packet->wheelSlip[i] = physicsData->wheelSlip[i];
		packet->wheelLoad[i] = physicsData->wheelLoad[i];
		packet->wheelsPressure[i] = physicsData->wheelsPressure[i];
		packet->wheelAngularSpeed[i] = physicsData->wheelAngularSpeed[i];
		packet->tyreWear[i] = physicsData->tyreWear[i];
		packet->tyreDirtyLevel[i] = physicsData->tyreDirtyLevel[i];
		packet->tyreCoreTemperature[i] = physicsData->tyreCoreTemperature[i];
		packet->camberRAD[i] = physicsData->camberRAD[i];
		packet->suspensionTravel[i] = physicsData->suspensionTravel[i];
		packet->tyreTempI[i] = physicsData->tyreTempI[i];
		packet->tyreTempM[i] = physicsData->tyreTempM[i];
		packet->tyreTempO[i] = physicsData->tyreTempO[i];
		packet->suspensionDamage[i] = physicsData->suspensionDamage[i];
		packet->tyreTemp[i] = physicsData->tyreTemp[i];
		packet->brakePressure[i] = physicsData->brakePressure[i];
		packet->padLife[i] = physicsData->padLife[i];
		packet->discLife[i] = physicsData->discLife[i];
	}

	packet->brakeBias = physicsData->brakeBias;
	packet->tcinAction = physicsData->tcinAction;
	packet->absInAction = physicsData->absInAction;
	packet->waterTemp = physicsData->waterTemp;
}
