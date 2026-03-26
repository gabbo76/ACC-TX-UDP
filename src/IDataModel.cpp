#include "../include/IDataModel.hpp"

std::shared_mutex IDataModel::_dataMutex;
std::mutex IDataModel::_clientsMutex;
std::mutex IDataModel::_instanceMutex;