#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

constexpr int  DEFAULT_SERVER_PORT = 9999;
constexpr int  DEFAULT_UPDATE_HZ = 60;
constexpr char CONFIG_FILE_PATH[] = "config.ini";
#define DEFAULT_SIM_TYPE "ACC"

struct Config {
    int serverPort;  // Port used both to receive START/STOP and send telemetry
    int updateHz;    // Telemetry update frequency in Hz
    std::string simType;
    int sleepMs() const { return 1000 / updateHz; }
};

class ConfigManager {
public:
    static ConfigManager& getInstance();
    ConfigManager(const ConfigManager&) = delete;
    void operator=(const ConfigManager&) = delete;

    // Loads config from file. If file doesn't exist, creates it with defaults.
    bool load();

    // Even if it's not thread-safe, the load function gets called once, so the Config struct is not overwritten.
    const Config& get() const { return _config; }

private:
    ConfigManager() {}

    void setDefaults();
    bool writeDefaults();

    Config _config{};
};

#endif