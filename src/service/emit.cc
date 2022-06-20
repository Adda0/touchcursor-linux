#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <cstring>
#include <iostream>

#include "binding.h"
#include "emit.h"
//#include "../tests/test.cc"

// String for the full key event output
char outputString[256];

// String for the emit function output
char emitString[8];

/**
 * Emits a key event.
 * */
int emit(int type, int code, int value)
{
#ifdef TEST
   std::cout << "emit: code=" << code << " value=" << value << "\n";
   sprintf(emitString, "%i:%i ", code, value);
   strcat(outputString, emitString);
#else
   struct input_event e{};
   e.time.tv_sec = 0;
   e.time.tv_usec = 0;
   // Set the virtual key code / value
   e.type = type;
   e.code = code;
   e.value = value;
   write(output, &e, sizeof(e));
   // Emit a syn event
   e.type = EV_SYN;
   e.code = SYN_REPORT;
   e.value = 0;
   write(output, &e, sizeof(e));
#endif
   return 0;
}
