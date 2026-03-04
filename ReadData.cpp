#include "ReadData.h"

#define MAX_ATTEMPTS 10

struct SMElement
{
	HANDLE hMapFile;
	unsigned char* mapFileBuffer;
};

SMElement m_graphics;
SMElement m_physics;
SMElement m_static;

int InitSM()
{
	TCHAR szPhysics[] = TEXT("Local\\acpmf_physics");
	TCHAR szGraphics[] = TEXT("Local\\acpmf_graphics");
	TCHAR szStatic[] = TEXT("Local\\acpmf_static");

	//INIT PHYSICS SHARED MEMORY
	m_physics.hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, szPhysics);
	if (!m_physics.hMapFile)
	{
		return -1;
	}
	m_physics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_physics.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFilePhysics));
	if (!m_physics.mapFileBuffer)
	{
		return -1;
	}

	//INIT GRAPHIC SHARED MEMORY
	m_graphics.hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, szGraphics);
	if (!m_graphics.hMapFile)
	{
		return -1;
	}
	m_graphics.mapFileBuffer = (unsigned char*)MapViewOfFile(m_graphics.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFileGraphic));
	if (!m_graphics.mapFileBuffer)
	{
		return -1;
	}

	//INIT STATIC SHARED MEMORY
	m_static.hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, szStatic);
	if (!m_static.hMapFile)
	{
		return -1;
	}
	m_static.mapFileBuffer = (unsigned char*)MapViewOfFile(m_static.hMapFile, FILE_MAP_READ, 0, 0, sizeof(SPageFileStatic));
	if (!m_static.mapFileBuffer)
	{
		return -1;
	}

	return 0;
}

void DismissSM()
{
	// Dismiss Physics
	if (m_physics.mapFileBuffer) UnmapViewOfFile(m_physics.mapFileBuffer);
	if (m_physics.hMapFile) CloseHandle(m_physics.hMapFile);
	m_physics.mapFileBuffer = NULL;
	m_physics.hMapFile = NULL;

	// Dismiss Graphics
	if (m_graphics.mapFileBuffer) UnmapViewOfFile(m_graphics.mapFileBuffer);
	if (m_graphics.hMapFile) CloseHandle(m_graphics.hMapFile);
	m_graphics.mapFileBuffer = NULL;
	m_graphics.hMapFile = NULL;

	// Dismiss Static
	if (m_static.mapFileBuffer) UnmapViewOfFile(m_static.mapFileBuffer);
	if (m_static.hMapFile) CloseHandle(m_static.hMapFile);
	m_static.mapFileBuffer = NULL;
	m_static.hMapFile = NULL;
}

void ReadPhysics(SPageFilePhysics* outData)
{
	// Controllo di sicurezza: se mi passi un puntatore nullo, esco subito
	if (!outData || !m_physics.mapFileBuffer) return;

	SPageFilePhysics* source = (SPageFilePhysics*)m_physics.mapFileBuffer;

	int retries = 0;
	while (retries < MAX_ATTEMPTS)
	{
		int startId = source->packetId;

		// Copiamo direttamente nella memoria dell'utente (outData)
		// Nessuna variabile locale intermedia necessaria per lo storage
		memcpy(outData, source, sizeof(SPageFilePhysics));

		int endId = source->packetId;

		if (startId == endId) return; // Copia riuscita
		retries++;
	}
}

void ReadGraphics(SPageFileGraphic* outData)
{
	if (!outData || !m_graphics.mapFileBuffer) return;

	SPageFileGraphic* source = (SPageFileGraphic*)m_graphics.mapFileBuffer;

	int retries = 0;
	while (retries < MAX_ATTEMPTS)
	{
		int startId = source->packetId;
		memcpy(outData, source, sizeof(SPageFileGraphic));
		int endId = source->packetId;

		if (startId == endId) return;
		retries++;
	}
}

void ReadStatic(SPageFileStatic* outData)
{
	if (!outData || !m_static.mapFileBuffer) return;

	// Per i dati statici basta una copia singola
	memcpy(outData, m_static.mapFileBuffer, sizeof(SPageFileStatic));
}