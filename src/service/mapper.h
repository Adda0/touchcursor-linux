#ifndef mapper_h
#define mapper_h

// The state machine states
enum states {
    idle,
    hyper,
    delay,
    map
};

extern enum states state;

/**
 * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(Config &config, int type, int code, int value);

void emitPermanentRemapping(Config &config, int type, int code, int value);

void emitHyperBinding(Config &config, int type, int hyperKeyCode, int originalKeyCode, int value);

#endif
