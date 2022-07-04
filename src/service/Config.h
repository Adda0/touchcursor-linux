#ifndef config_h
#define config_h

#include "Bindings.h"

/**
 * The event path for the device.
 * */
extern char eventPath[18];

class Config {
public:
    Bindings bindings{};

    bool isDebugMode() { return debugMode; }

    void setDebugMode() { debugMode = true; }

    /**
     * Parses configuration file and initializes an object of Config class.
     * @param configPath Path to the configuration file.
     * @return Config object configurad from given configuration file.
     */
    static Config fromConfigFile(const std::string &configPath = "");

private:
    bool debugMode{false};

    static Config parseConfigurationFile(FILE *configFile);

    static FILE *findConfigurationFile(const std::string &configPath);
};

#endif
