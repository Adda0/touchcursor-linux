#ifndef config_h
#define config_h

#include "Bindings.h"

/**
 * The event path for the device.
 * */
extern char eventPath[18];

/**
 * Reads the configuration file.
 * */
Bindings readConfiguration(const std::string& configPath = "");

#endif
