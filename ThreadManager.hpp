#ifndef THREAD_MANAGER_HPP
#define THREAD_MANAGER_HPP

#include <vector>
#include <thread>
#include <mutex>

class ThreadManager {
private:
    
    std::vector<std::thread> registry;

	// Mutex for the registry access
    std::mutex registryMtx;

    // Singleton stuffs
    static ThreadManager* instance;
    static std::mutex instanceMtx;

    // Private constructor
    ThreadManager() {}

public:
    static ThreadManager& getInstance();

	// Methods to manage the threads
    void addThread(std::thread&& t);
    std::vector<std::thread>& getRegistry();
    void joinAll();

    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;
};

#endif