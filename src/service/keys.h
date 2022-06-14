#ifndef keys_h
#define keys_h

/**
 * Checks if the event is a key down.
 * */
int isDown(int value);

/**
 * Checks if the key is a keypad key.
 * */
int isKeypad(int code);

/**
 * Checks if the key is a modifier key.
 * */
int isModifier(int code);

#endif
