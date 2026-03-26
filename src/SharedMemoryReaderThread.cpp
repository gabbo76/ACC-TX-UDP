#pragma once

#include "../include/SharedMemoryReaderThread.hpp"
#include "../include/DataFactory.hpp"

void readSharedMemoryThread(std::atomic<bool>& exit) {

	int sleepMs = ConfigManager::getInstance().get().sleepMs();

	std::unique_ptr<DataFactory> factory = DataFactory::getFactory(ConfigManager::getInstance().get().simType);

	void* data = NULL;

	if(ConfigManager::getInstance().get().simType == "ACC") {
		// Allocate data to host data for ACC
		data = malloc(sizeof(ACCData));
	}

	IDataModel& model = factory->getModel();
	DataReaders& reader = factory->getReader();

	while (!exit) {

		reader.ReadData(data);
		model.updateData(data);

		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
	}
}
