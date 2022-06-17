#ifndef config_h
#define config_h

#include "Bindings.h"

/**
 * The event path for the device.
 * */
extern char eventPath[18];

/**
 * The hyper key;
 * */
extern int hyperKey;

/**
 * Map for keys and their conversion.
 * */
extern int keymap[256];

/**
 * Map for permanently remapped keys.
 * */
extern int remap[256];

/**
 * Reads the configuration file.
 * */
Bindings readConfiguration(const std::string& configPath = "");

#endif
