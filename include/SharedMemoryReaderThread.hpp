//#include "DataModel.hpp"
#include <atomic>
#include "GlobalDebug.hpp"
#include "Config.hpp"
#include "DataReaders.hpp"
#include <iostream>
#include <sstream>
#include <fstream>


void readSharedMemoryThread(std::atomic<bool>& exit);