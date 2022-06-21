#include <cstdio>
#include <cstring>
#include <iostream>

#include "../service/emit.h"

// String for the full key event output
char outputString[256];

// String for the emit function output
char emitString[8];

/**
 * Emits a key event.
 * */
int emit(int type, int code, int value)
{
    std::cout << "emit: code=" << code << " value=" << value << "\n";
    sprintf(emitString, "%i:%i ", code, value);
    strcat(outputString, emitString);

    return 0;
}
