#ifndef DATAFACTORY_H
#define DATAFACTORY_H

#include "../DataReaders.hpp"
#include "../IDataModel.hpp"
#include "../include/DataFactory.hpp"

class ACCFactory : public DataFactory {
public:
	IDataModel& getModel();
	DataReaders& getReader();
	ACCFactory() {}
};

#endif