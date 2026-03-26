#pragma once

#include "IDataModel.hpp"
#include "DataReaders.hpp"
#include <string>
#include <memory>


class DataFactory {
public:
	virtual IDataModel& getModel() = 0;
	virtual DataReaders& getReader() = 0;
	static std::unique_ptr<DataFactory> getFactory(std::string sim_type);
};