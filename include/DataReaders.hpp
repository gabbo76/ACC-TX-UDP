#pragma once

#define MAX_ATTEMPTS 10
#include "Models/ACC/SharedFileOut.h"

typedef struct ACCData {
	SPageFileGraphic g;
	SPageFilePhysics p;
	SPageFileStatic s;
} ACCData;

typedef struct R3EData {
	// TODO
} R3EData;;

class DataReaders {
public: 
	virtual void ReadData(void* outData) = 0;
	virtual int InitSM() = 0;
	virtual void DismissSM() = 0;

	void operator=(const DataReaders&) = delete;

};