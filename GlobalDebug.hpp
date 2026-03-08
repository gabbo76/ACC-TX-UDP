#ifndef GLOBAL_DEBUG_HPP
#define GLOBAL_DEBUG_HPP
#include <fstream>

inline void LogToFile(const std::string& msg) {
	std::ofstream logFile("debug_shutdown.txt", std::ios::app);
	if (logFile.is_open()) {
		logFile << msg << std::endl;
		logFile.close();
	}
}

#endif