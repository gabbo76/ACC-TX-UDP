#include "SharedMemoryReaderThread.hpp"

void readSharedMemoryThread(std::atomic<bool>& exit) {
	
	std::cout << "Shared Memory Reader Thread started." << std::endl;
	std::stringstream ss;
	ss << std::this_thread::get_id();
	std::string threadIdStr = ss.str();
	LogToFile("[S.M. Reader] Thread " + threadIdStr + ".");

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
