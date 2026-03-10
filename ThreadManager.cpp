#include "ThreadManager.hpp"
#include <iostream>
#include <sstream>

ThreadManager* ThreadManager::instance = nullptr;
std::mutex ThreadManager::instanceMtx;

ThreadManager& ThreadManager::getInstance() {
    // Double-checked locking per essere sicuri al 100% tra i vari thread
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(instanceMtx);
        if (instance == nullptr) {
            instance = new ThreadManager();
        }
    }
    return *instance;
}

void ThreadManager::addThread(std::thread&& t) {
    if (t.joinable()) {
        std::lock_guard<std::mutex> lock(registryMtx);
        
        // Debug
        std::stringstream ss;
        ss << t.get_id();
        std::string tid = ss.str();

        registry.push_back(std::move(t));

        //std::cout << "[THREAD MANAGER] Thread " << tid << " aggiunto. Totale: " << registry.size() << std::endl;
    }
}

std::vector<std::thread>& ThreadManager::getRegistry() {
    std::lock_guard<std::mutex> lock(registryMtx);
    return registry;
}