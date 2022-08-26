#ifndef mapper_h
#define mapper_h

/// The state machine states TouchCursor can find itself in.
enum class States {
    /// No hyper keys are pressed. TouchCursor waits for a hyper key DOWN event. Permanent hyper bindings are being
    ///  emitted, otherwise normal key codes are emitted.
    idle,

    /// Hyper key has been pressed DOWN (and is currently held pressed down) and TouchCursor monitors for potential
    ///  hyper key bindings to emit.
    hyper,

    /// Hyper key and some mapped key were pressed down. However, in order to affect the normal keyboard behaviour as
    ///  little as possible, we wait for another key press before emitting a binding value of the mapped key.
    delay,

    /// Mapped key has been emitted and we are able to emit next bindings. Applicable when holding mapped key pressed down
    ///  for a period of time or pressing multiple mapped keys.
    map,
};

extern Config config; ///< Loaded configuration for the current instance of TouchCursor.

/**
 * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(int type, int code, int value);

void emitPermanentRemapping(int type, int code, int value);

void emitHyperBinding(int type, int hyperKeyCode, int originalKeyCode, int value);

#endif
