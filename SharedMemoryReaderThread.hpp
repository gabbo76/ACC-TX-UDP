#include "DataModel.hpp"
#include <atomic>
#include "GlobalDebug.hpp"
#include "ReadData.h"
#include "SharedFileOut.h"
#include <iostream>
#include <sstream>
#include <fstream>


void readSharedMemoryThread(std::atomic<bool>& exit);