#pragma once

#define MAX_ATTEMPTS 10
#include "SharedFileOut.h"

typedef struct ACCData {
	SPageFileGraphic g;
	SPageFilePhysics p;
	SPageFileStatic s;
} ACCData;

class DataReaders {
public: 
	virtual void ReadData(void* outData) = 0;
	virtual int InitSM() = 0;
	virtual void DismissSM() = 0;

	void operator=(const DataReaders&) = delete;

};