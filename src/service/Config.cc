//#define _GNU_SOURCE
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <ranges>
#include <iostream>

#include "Config.h"
#include "KeyCodes.h"
#include "Bindings.h"

char eventPath[18];

/**
 * @brief Trim comment from the end of the string started by '#' character.
 *
 * @param s String to be trimmed.
 * @return char* Trimmed string without any comments.
 * */
char* trimComment(char* s)
{
    if (s != NULL)
    {
        char *p  = strchr(s, '#');
        if (p != NULL)
        {
            // p points to the start of the comment.
            *p = '\0';
        }
    }
    return s;
}

/**
 * Trims a string.
 * credit to chux: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way#122721
 * */
char* trimString(char* s)
{
    while (isspace((unsigned char)*s)) s++;
    if (*s)
    {
        char *p = s;
        while (*p) p++;
        while (isspace((unsigned char) *(--p)));
        p[1] = '\0';
    }
    return s;
}

/**
 * Checks if the string starts with a specific value.
 */
int startsWith(const char* str, const char* value)
{
    return strncmp(str, value, strlen(value)) == 0;
}

/**
 * Checks for the device number if it is configured.
 */
int getDeviceNumber(char* deviceConfigValue)
{
    int deviceNumber = 1;
    int length = strlen(deviceConfigValue);
    for (int i = length - 1; i >= 0; i--)
    {
        if (deviceConfigValue[i] == '\0') break;
        if (deviceConfigValue[i] == '"') break;
        if (deviceConfigValue[i] == ':')
        {
            deviceNumber = atoi(deviceConfigValue + i + 1);
            deviceConfigValue[i] = '\0';
        }
    }
    return deviceNumber;
}

/**
 * Checks for commented or empty lines.
 */
int isCommentOrEmpty(char* line)
{
    return line[0] == '#' || line[0] == '\0';
}

/**
 * Searches /proc/bus/input/devices for the device event.
 */
void findDeviceEvent(char* deviceConfigValue)
{
    eventPath[0] = '\0';

    char* deviceName = deviceConfigValue;
    int deviceNumber = getDeviceNumber(deviceName);

    fprintf(stdout, "info: deviceName: %s\n", deviceName);
    fprintf(stdout, "info: deviceNumber: %i\n", deviceNumber);

    std::string devicesFilePath { "/proc/bus/input/devices" };
    FILE* devicesFile = fopen(devicesFilePath.c_str(), "r");
    if (!devicesFile)
    {
        fprintf(stderr, "error: could not open /proc/bus/input/devices\n");
        return;
    }

    char* line = NULL;
    int matchedName = 0;
    int matchedCount = 0;
    int foundEvent = 0;
    size_t length = 0;
    ssize_t result;
    while (!foundEvent && (result = getline(&line, &length, devicesFile)) != -1)
    {
        if (length < 3) continue;
        if (isspace(line[0])) continue;
        if (!matchedName)
        {
            if (!startsWith(line, "N: ")) continue;
            char* trimmedLine = trimString(line + 3);
            if (strcmp(trimmedLine, deviceName) == 0)
            {
                if (deviceNumber == ++matchedCount)
                {
                    matchedName = 1;
                }
                continue;
            }
        }
        if (matchedName)
        {
            if (!startsWith(line, "H: Handlers")) continue;
            char* tokens = line;
            char* token = strsep(&tokens, "=");
            while (tokens != NULL)
            {
                token = strsep(&tokens, " ");
                if (startsWith(token, "event"))
                {
                    strcat(eventPath, "/dev/input/");
                    strcat(eventPath, token);
                    fprintf(stdout, "info: found the keyboard event\n");
                    foundEvent = 1;
                    break;
                }
            }
        }
    }

    if (!foundEvent)
    {
        fprintf(stderr, "error: could not find device: %s\n", deviceConfigValue);
    }

    fclose(devicesFile);
    if (line) free(line);
}

static enum sections
{
    configuration_none,
    configuration_device,
    configuration_remap,
    configuration_hyper,
    configuration_bindings
} section;

/**
 * Helper method to print existing keyboard devices.
 * Does not work for bluetooth keyboards.
 */
// Need to revisit this
// void printKeyboardDevices()
// {
//     DIR* directoryStream = opendir("/dev/input/");
//     if (!directoryStream)
//     {
//         printf("error: could not open /dev/input/\n");
//         return; //EXIT_FAILURE;
//     }
//     fprintf(stdout, "suggestion: use any of the following in the configuration file for this application:\n");
//     struct dirent* directory = NULL;
//     while ((directory = readdir(directoryStream)))
//     {
//         if (strstr(directory->d_name, "kbd"))
//         {
//             printf ("keyboard=/dev/input/by-id/%s\n", directory->d_name);
//         }
//     }
// }

Config Config::fromConfigFile(const std::string& configPath) {
    char configFilePath[256];
    // Find the configuration file.
    configFilePath[0] = '\0';
    FILE* configFile;

    if (configPath.empty()) {
        char* homePath = getenv("HOME");
        if (!homePath) {
            fprintf(stderr, "error: home path environment variable not specified\n");
        }
        if (homePath) {
            strcat(configFilePath, homePath);
            strcat(configFilePath, "/.config/touchcursor/touchcursor.conf");
            fprintf(stdout, "info: looking for the configuration file at: %s\n", configFilePath);
            configFile = fopen(configFilePath, "r");
        }
        if (!configFile) {
            strcpy(configFilePath, "/etc/touchcursor/touchcursor.conf");
            fprintf(stdout, "info: looking for the configuration file at: %s\n", configFilePath);
            configFile = fopen(configFilePath, "r");
        }
        if (!configFile) {
            fprintf(stderr, "error: could not open the configuration file\n");
            return Config{};
        }
    } else {
        fprintf(stdout, "info: looking for the configuration file at: %s\n", configPath.c_str());
        configFile = fopen(configPath.c_str(), "r");
    }

    fprintf(stdout, "info: found the configuration file\n");

    // Parse the configuration file.
    Config config{};
    char* buffer = NULL;
    size_t length = 0;
    ssize_t result = -1;
    std::string current_hyper_key{};

    while ((result = getline(&buffer, &length, configFile)) != -1)
    {
        char* line = trimComment(buffer);
        line = trimString(line);
        // Comment or empty line
        if (isCommentOrEmpty(line))
        {
            continue;
        }
        // Check for section
        if (strncmp(line, "[Device]", strlen(line)) == 0)
        {
            section = configuration_device;
            continue;
        }
        if (strncmp(line, "[Remap]", strlen(line)) == 0)
        {
            section = configuration_remap;
            continue;
        }
        if (strncmp(line, "[Hyper]", strlen(line)) == 0)
        {
            section = configuration_hyper;
            continue;
        }
        if (strncmp(line, "[Bindings]", strlen(line)) == 0)
        {
            section = configuration_bindings;
            continue;
        }

        KeyCodes keyCodes = KeyCodes{};

        // Read configurations
        //std::cout << "Read configurations.\n";
        switch (section)
        {
            case configuration_device:
            {
                if (eventPath[0] == '\0')
                {
                    findDeviceEvent(line);
                }
                continue;
            }
            case configuration_remap:
            {
                char* tokens = line;
                char* token = strsep(&tokens, "=");
                int fromCode = keyCodes.getKeyCodeFromKeyString(token);
                token = strsep(&tokens, "=");
                int toCode = keyCodes.getKeyCodeFromKeyString(token);
                config.bindings.addPermanentRemapping(fromCode, toCode);
                break;
            }
            case configuration_hyper:
            {
                char* tokens = line;
                char* hyperKeyName = strsep(&tokens, "=");
                char* token = strsep(&tokens, "=");
                int code = keyCodes.getKeyCodeFromKeyString(token);

                config.bindings.addHyperName(hyperKeyName, code);
                config.bindings.addHyperKey(code);
                break;
            }
            case configuration_bindings:
            {
                // Switch between common and specific hyper key configurations.
                std::string line_str = line;
                if (line_str.starts_with("[")) {
                    line_str.erase(0, 1);
                    line_str.erase(line_str.length() - 1);
                    line_str.erase(0, line_str.find_last_of('.') + 1);
                    current_hyper_key = line_str;
                    break;
                }

                char* token = strsep(&line, "=");
                int fromCode = keyCodes.getKeyCodeFromKeyString(token);
                token = strsep(&line, "=");
                int toCode = keyCodes.getKeyCodeFromKeyString(token);

                std::cout << "Current Hyper key empty: " << current_hyper_key.empty() << "\n";

                if (current_hyper_key.empty()) {
                    std:: cout << "Adding common hyper key mapping.\n";
                    config.bindings.addCommonHyperMapping(fromCode, toCode);
                    //std::cout << config.bindings.getMappedKeyForHyperBinding(hyperKey, fromCode) << "\n";
                } else {
                    std::cout << "Current hyper key: " << current_hyper_key << "\n";
                    config.bindings.addHyperMapping(config.bindings.getHyperKeyForHyperName(current_hyper_key), fromCode, toCode);
                    std::cout << "Added specific hyper key option.\n";
                }
                break;
            }
            case configuration_none:
            default:
            {
                continue;
            }
        }
    }

    fclose(configFile);
    if (buffer)
    {
        free(buffer);
    }

    config.bindings.bindHyperNamesWithoutKeys();

    return config;
}
