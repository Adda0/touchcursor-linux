#ifndef keys_h
#define keys_h

/**
 * @brief Checks if the event is key down.
 *
 * Linux input sends value=2 for repeated key down. We treat them as key down events for processing.
 */
bool isDown(int value);

/**
 * Checks if the event is a key up.
 */
bool isUp(int value);

/**
 * Checks if the key is a keypad key.
 */
int isKeypad(int code);

/**
 * Checks if the key is a modifier key.
 */
int isModifier(int code);

#endif
