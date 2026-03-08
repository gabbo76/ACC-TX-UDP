#ifndef THREAD_MANAGER_HPP
#define THREAD_MANAGER_HPP

#include <vector>
#include <thread>
#include <mutex>

class ThreadManager {
private:
    // Il contenitore fisico dei thread
    std::vector<std::thread> registry;

    // Mutex per evitare che Main e Handler si scontrino
    std::mutex registryMtx;

    // Singleton stuffs
    static ThreadManager* instance;
    static std::mutex instanceMtx;

    // Costruttore privato
    ThreadManager() {}

public:
    static ThreadManager& getInstance();

    // Metodi per gestire i thread
    void addThread(std::thread&& t);
    std::vector<std::thread>& getRegistry();

    // Eliminiamo copia e assegnazione
    ThreadManager(const ThreadManager&) = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;
};

#endif