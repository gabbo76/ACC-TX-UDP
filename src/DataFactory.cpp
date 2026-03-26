#include "../include/DataFactory.hpp"
#include "../include/Factory/ACCFactory.hpp"

std::unique_ptr<DataFactory> DataFactory::getFactory(std::string sim_type) {
	if (sim_type == "ACC") {
		return std::make_unique<ACCFactory>();
	}
	else {
		// exit with ecception
		throw std::runtime_error("Unknown sim: " + sim_type);
	}
	return nullptr;
}