//#define _GNU_SOURCE
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <linux/input.h>

#include "Config.h"
#include "binding.h"
#include "emit.h"
#include "mapper.h"

Config config;

/**
* Main method.
* */
int main(int argc, char *argv[]) {
    config = Config::fromConfigFile();
    if (eventPath[0] == '\0') {
        fprintf(stderr, "error: please specify the keyboard device name in the configuration file\n");
        return EXIT_FAILURE;
    }
    // Bind the input device
    if (bindInput(eventPath) != EXIT_SUCCESS) {
        fprintf(stderr, "error: could not capture the keyboard device\n");
        return EXIT_FAILURE;
    }
    // Bind the output device
    if (bindOutput() != EXIT_SUCCESS) {
        fprintf(stderr, "error: could not create the virtual keyboard device\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "info: running\n");
    // Read events
    struct input_event inputEvent;
    ssize_t result;
    while (true) {
        result = read(input, &inputEvent, sizeof(inputEvent));
        if (result == (ssize_t) -1) {
            if (errno == EINTR) continue;
        }
        if (result == (ssize_t) 0) {
            return ENOENT;
        }
        if (result != sizeof(inputEvent)) {
            return EIO;
        }
        // We only want to manipulate key presses
        if (inputEvent.type == EV_KEY
            && (inputEvent.value == 0 || inputEvent.value == 1 || inputEvent.value == 2)) {
            processKey(inputEvent.type, inputEvent.code, inputEvent.value);
        } else {
            emit(inputEvent.type, inputEvent.code, inputEvent.value);
        }
    }
}
