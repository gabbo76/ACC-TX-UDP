#pragma once

#include "../../include/SharedFileOut.h"
#include "../../include/DataReaders.hpp"
#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <mutex>


class ACCReader : public DataReaders {
public:
	
	ACCReader(const ACCReader&) = delete;

	static ACCReader& getInstance();

	int InitSM();

	void ReadData(void* outData) override;

	void DismissSM() override;

	void ReadPhysics(SPageFilePhysics* outData);

	void ReadGraphics(SPageFileGraphic* outData);

	void ReadStatic(SPageFileStatic* outData);


protected:
	struct SMElement
	{
		HANDLE hMapFile;
		unsigned char* mapFileBuffer;
	};

	SMElement m_graphics;
	SMElement m_physics;
	SMElement m_static;
private:
	ACCReader() : DataReaders() {}
};
