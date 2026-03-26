#include "../../include/Factory/ACCFactory.hpp"
#include "../../include/Models/ACCModel.hpp"
#include "../../include/DataReaders/ACCReader.hpp"

DataReaders& ACCFactory::getReader() {
	return ACCReader::getInstance();
}

IDataModel& ACCFactory::getModel(){
	return ACCModel::getInstance();
}
