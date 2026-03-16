#include "../include/DataModel.hpp"
#include <atomic>
#include "../include/GlobalDebug.hpp"
#include "../include/ReadData.h"
#include "../include/SharedFileOut.h"
#include "../include/Config.hpp"
#include <iostream>
#include <sstream>
#include <fstream>


void readSharedMemoryThread(std::atomic<bool>& exit);