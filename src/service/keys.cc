#include <linux/uinput.h>

#include "keys.h"

/**
 * Checks if the event is key down.
 * Linux input sends value=2 for repeated key down.
 * We treat them as keydown events for processing.
 * */
int isDown(int value)
{
    return value == 1 || value == 2;
}

/**
 * Checks if the key is a keypad key.
 * */
int isKeypad(int code)
{
    switch (code)
    {
        case KEY_KPASTERISK:
        case KEY_KP7:
        case KEY_KP8:
        case KEY_KP9:
        case KEY_KPMINUS:
        case KEY_KP4:
        case KEY_KP5:
        case KEY_KP6:
        case KEY_KPPLUS:
        case KEY_KP1:
        case KEY_KP2:
        case KEY_KP3:
        case KEY_KP0:
        case KEY_KPDOT:
            {
                return 1;
            }
        default:
            {
                return 0;
            }
    }
}

/**
 * Checks if the key is a modifier key.
 * */
int isModifier(int code)
{
    switch (code)
    {
        case KEY_LEFTSHIFT:
        case KEY_RIGHTSHIFT:
        case KEY_LEFTCTRL:
        case KEY_RIGHTCTRL:
        case KEY_LEFTALT:
        case KEY_RIGHTALT:
        case KEY_LEFTMETA:
        case KEY_RIGHTMETA:
        case KEY_CAPSLOCK:
        case KEY_NUMLOCK:
        case KEY_SCROLLLOCK:
            {
                return 1;
            }
        default:
            {
                return 0;
            }
    }
}
