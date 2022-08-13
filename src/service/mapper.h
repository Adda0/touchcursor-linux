#ifndef mapper_h
#define mapper_h

/// The state machine states.
enum class States {
    /// No hyper keys are pressed. TouchCursor waits for a hyper key DOWN event. Permanent hyper bindings are being
    /// emitted, otherwise normal key codes are emitted.
    idle,

    /// Hyper key has been pressed DOWN (and is currently held pressed down) and TouchCursor monitors for potential
    /// hyper key bindings to emit.
    hyper,

    delay,

    map,
};

extern Config config;

/**
 * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(int type, int code, int value);

void emitPermanentRemapping(int type, int code, int value);

void emitHyperBinding(int type, int hyperKeyCode, int originalKeyCode, int value);

#endif
