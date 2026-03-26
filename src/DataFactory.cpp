#include "../include/DataFactory.hpp"
#include "../include/Factory/ACCFactory.hpp"

std::unique_ptr<DataFactory> DataFactory::getFactory(std::string sim_type) {
	if (sim_type == "ACC") {
		return std::make_unique<ACCFactory>();
	}
	// Add more simulation types here as needed
	return nullptr;
}