#include "../include/SharedMemoryReaderThread.hpp"

void readSharedMemoryThread(std::atomic<bool>& exit) {

	int sleepMs = ConfigManager::getInstance().get().sleepMs();

	SPageFileGraphic g;
	SPageFilePhysics p;
	SPageFileStatic s;

	while (!exit) {

		ReadPhysics(&p);
		ReadGraphics(&g);
		ReadStatic(&s);

		DataModel::getInstance().updateData(g, p, s);

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs)); // Sleep for 16ms to achieve ~60Hz update rateù
	}
}
