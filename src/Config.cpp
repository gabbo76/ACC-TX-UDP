#include "../include/Config.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::setDefaults() {
    _config.serverPort = DEFAULT_SERVER_PORT;
    _config.updateHz = DEFAULT_UPDATE_HZ;
    _config.simType = DEFAULT_SIM_TYPE; // Inizializziamo il default
}

bool ConfigManager::writeDefaults() {
    std::ofstream f(CONFIG_FILE_PATH);
    if (!f.is_open()) {
        std::cerr << "[CONFIG] Could not create config file: " << CONFIG_FILE_PATH << std::endl;
        return false;
    }

    f << "[network]\n";
    f << "serverPort=" << DEFAULT_SERVER_PORT << "\n";
    f << "\n";
    f << "[telemetry]\n";
    f << "updateHz=" << DEFAULT_UPDATE_HZ << "\n";
    f << "\n";
    f << "[sim]\n";
    f << "simType=" << DEFAULT_SIM_TYPE << "\n";

    std::cout << "[CONFIG] Config file created with defaults: " << CONFIG_FILE_PATH << std::endl;
    return true;
}

bool ConfigManager::load() {
    setDefaults();

    std::ifstream f(CONFIG_FILE_PATH);
    if (!f.is_open()) {
        std::cout << "[CONFIG] Config file not found, creating with defaults..." << std::endl;
        return writeDefaults();
    }

    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#' || line[0] == '[')
            continue;

        auto sep = line.find('=');
        if (sep == std::string::npos)
            continue;

        std::string key = line.substr(0, sep);
        std::string value = line.substr(sep + 1);

        try {
            if (key == "serverPort") _config.serverPort = std::stoi(value);
            else if (key == "updateHz")   _config.updateHz = std::stoi(value);
            else if (key == "simType")   _config.simType = value;
        }
        catch (const std::exception&) {
            std::cerr << "[CONFIG] Invalid value for key '" << key << "', using default." << std::endl;
        }
    }

    if (_config.updateHz < 1 || _config.updateHz > 120) {
        std::cerr << "[CONFIG] updateHz out of range (1-120), using default: "
            << DEFAULT_UPDATE_HZ << std::endl;
        _config.updateHz = DEFAULT_UPDATE_HZ;
    }

    std::cout << "[CONFIG] Loaded: serverPort=" << _config.serverPort
        << " updateHz=" << _config.updateHz << " sim=" << _config.simType << std::endl;

    return true;
}