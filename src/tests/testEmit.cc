#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstdarg>
#include <linux/input.h>

#include <catch2/catch_test_macros.hpp>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/Config.h"
#include "../service/mapper.h"

// minunit http://www.jera.com/techinfo/jtns/jtn002.html
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { int result = test(); tests_run++; if (result != 0) return result; } while (0)

extern char outputString[256];

Config config;

/*
 * Simulates typing keys.
 * The method arguments should be number of arguments, then pairs of key code and key value.
 */
static void type(int num, ...) {
    for (int i = 0; i < 256; i++) outputString[i] = 0;
    va_list arguments;
    va_start(arguments, num);
    for (int i = 0; i < num; i += 2) {
        int code = va_arg(arguments, int);
        int value = va_arg(arguments, int);
        processKey(EV_KEY, code, value);
    }
    va_end(arguments);
}

TEST_CASE("Emit events according to configuration", "[emit]") {
    KeyCodes keyCodes = KeyCodes{};

    TOriginalKey originalKey{keyCodes.getKeyCodeFromKeyString("KEY_H")};
    TMappedKey mappedKey{keyCodes.getKeyCodeFromKeyString("KEY_G")};
    std::string expected{};

    SECTION("Emit according to config_files/simple_config_multiple_hyper_keys.conf") {
        std::filesystem::path configPath =
                std::filesystem::current_path() / "src/tests/config_files/simple_config_multiple_hyper_keys.conf";

        config = Config::fromConfigFile(std::string{configPath});

        SECTION("Normal typing without overlapping key events") {
            SECTION("Space down, up") {
                expected = "57:1 57:0 ";
                type(4, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, up, down, up") {
                expected = "57:1 57:0 57:1 57:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_SPACE, EVENT_KEY_DOWN,
                     KEY_SPACE, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Other down, Space down, up, Other up") {
                expected = "31:1 57:1 57:0 31:0 ";
                type(8, KEY_S, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_S,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, Other down, up, Space up") {
                expected = "57:1 31:1 31:0 57:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_S, EVENT_KEY_DOWN, KEY_S, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, Other down, up, Mapped down, up, Space up") {
                expected = "57:1 31:1 31:0 34:1 34:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_S, EVENT_KEY_DOWN, KEY_S, EVENT_KEY_UP, KEY_H, EVENT_KEY_DOWN, KEY_H, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("V down, up") {
                expected = "47:1 47:0 ";
                type(4, KEY_V, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, common mapped H down, common mapped H up, space up") {
                expected = "34:1 34:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_H, EVENT_KEY_DOWN, KEY_H, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, Mapped down, up, Space up") {
                expected = "105:1 105:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Mapped down, Space down, up, Mapped up") {
                expected = "36:1 57:1 57:0 36:0 ";
                type(8, KEY_J, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_J,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Mapped down, Space down, up, Different mapped down, up, Mapped up") {
                expected = "36:1 57:1 57:0 23:1 23:0 36:0 ";
                type(12, KEY_J, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_I,
                     EVENT_KEY_DOWN, KEY_I, EVENT_KEY_UP, KEY_J, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("V down, Mapped down, up, V up") {
                expected = "105:1 105:0 ";
                type(8, KEY_V, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_V, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("V down, Mapped down, up, Mapped down, up, V up") {
                expected = "105:1 105:0 105:1 105:0 ";
                type(12, KEY_V, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_J,
                     EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_V, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

                // TODO: Correctly set what to expect.
            SECTION("V down, Space down, Space up, V up") {
                expected = "47:1 57:1 57:0 47:0 ";
                type(8, KEY_V, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_V,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, V down, V up, Space up") {
                expected = "45:1 45:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, V down, V up, V down, V up, Space up") {
                expected = "45:1 45:0 45:1 45:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_UP, KEY_V,
                     EVENT_KEY_DOWN, KEY_V, EVENT_KEY_UP, KEY_SPACE, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Shift down, V down, V up, Shift up") {
                expected = "42:1 47:1 47:0 42:0 ";
                type(8, KEY_LEFTSHIFT, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_DOWN, KEY_V, EVENT_KEY_UP,
                     KEY_LEFTSHIFT, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }
        }

        SECTION("Fast typing with overlapping key events") {

            SECTION("Space down, mapped down, space up, mapped up") {
                // The mapped keys should not be converted. The normal behaviour is to emit the codes in the order as
                // they come.
                expected = "57:1 36:1 57:0 36:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_J,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Mapped down, space down, mapped up, space up") {
                // This is not out of order. Remember space down does not emit anything.
                // The mapped keys should not be converted.
                expected = "36:1 36:0 57:1 57:0 ";
                type(8, KEY_J, EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_SPACE,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, mapped1 down, mapped2 down, space up, mapped1 up, mapped2 up") {
                // The mapped keys should be sent converted.
                // Extra up events are sent, but that does not matter.
                expected = "105:1 103:1 105:0 103:0 36:0 23:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_SPACE,
                     EVENT_KEY_UP, KEY_J, EVENT_KEY_UP, KEY_I, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            // TODO: Set
            SECTION("Space down, mapped1 down, mapped2 down, mapped2 down, space up, mapped1 up, mapped2 up") {
                // The mapped keys should be sent converted.
                // Extra up events are sent, but that does not matter.
                expected = "105:1 103:1 105:0 103:0 36:0 23:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_SPACE,
                     EVENT_KEY_UP, KEY_J, EVENT_KEY_UP, KEY_I, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            // TODO: Set
            SECTION("Space down, mapped1 down, mapped2 down, mapped2 up, space up, mapped1 up, mapped2 up") {
                // The mapped keys should be sent converted.
                // Extra up events are sent, but that does not matter.
                expected = "105:1 103:1 105:0 103:0 36:0 23:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_SPACE,
                     EVENT_KEY_UP, KEY_J, EVENT_KEY_UP, KEY_I, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

                // TODO: Set
            SECTION("Space down, mapped1 down, mapped2 down, mapped3 dow, space up, mapped1 up, mapped2 up") {
                // The mapped keys should be sent converted.
                // Extra up events are sent, but that does not matter.
                expected = "105:1 103:1 105:0 103:0 36:0 23:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_SPACE,
                     EVENT_KEY_UP, KEY_J, EVENT_KEY_UP, KEY_I, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, mapped1 down, mapped2 down, mapped3 down, space up, mapped1 up, mapped2 up, mapped3 up") {
                // The mapped keys should be sent converted.
                // Extra up events are sent, but that does not matter.
                expected = "105:1 103:1 106:1 105:0 103:0 106:0 36:0 23:0 38:0 ";
                type(16, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_L,
                     EVENT_KEY_DOWN, KEY_SPACE, EVENT_KEY_UP, KEY_J, EVENT_KEY_UP, KEY_I, EVENT_KEY_UP, KEY_L,
                     EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }
        }

        SECTION("Special typing with overlapping key events") {

            SECTION("Space down, other (modifier) down, other (modifier) up, space up") {
                // The mapped keys should not be converted.
                expected = "57:1 42:1 42:0 57:0 ";
                type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_LEFTSHIFT, EVENT_KEY_DOWN, KEY_LEFTSHIFT, EVENT_KEY_UP,
                     KEY_SPACE, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }

            SECTION("Space down, other (modifier) down, mapped down, up, other (modifier) up, space up") {
                // The mapped keys should not be converted.
                expected = "42:1 25:1 25:0 42:0 ";
                type(12, KEY_SPACE, EVENT_KEY_DOWN, KEY_LEFTSHIFT, EVENT_KEY_DOWN, KEY_U, EVENT_KEY_DOWN, KEY_U,
                     EVENT_KEY_UP, KEY_LEFTSHIFT, EVENT_KEY_UP, KEY_SPACE, EVENT_KEY_UP);
                REQUIRE(expected == outputString);
            }
        }
    }

    SECTION("Emit according to config_files/sequence_combination.conf") {
        std::filesystem::path configPath =
                std::filesystem::current_path() / "src/tests/config_files/sequence_combination.conf";

        config = Config::fromConfigFile(std::string{configPath});

        SECTION("Combination") {
            expected = "42:1 34:1 34:0 42:0 ";
            type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_H, EVENT_KEY_DOWN, KEY_H, EVENT_KEY_UP, KEY_SPACE,
                 EVENT_KEY_UP);
            REQUIRE(expected == outputString);
        }

        SECTION("Sequence") {
            expected = "20:1 20:0 33:1 33:0 ";
            type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_DOWN, KEY_J, EVENT_KEY_UP, KEY_SPACE,
                 EVENT_KEY_UP);
            REQUIRE(expected == outputString);
        }

        SECTION("Sequence + combination") {
            expected = "103:1 103:0 42:1 34:1 34:0 42:0 33:1 33:0 ";
            type(8, KEY_SPACE, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_DOWN, KEY_I, EVENT_KEY_UP, KEY_SPACE,
                 EVENT_KEY_UP);
            REQUIRE(expected == outputString);
        }
    }
}

// Sample tests from touchcursor source

// normal (slow) typing
// CHECK((SP, up,  SP,up, 0));
// CHECK((SP, dn,  SP,up, 0));
// CHECK((SP, up,  SP,up, SP,dn, SP,up, 0));
// CHECK((x, dn,   SP,up, SP,dn, SP,up, x,dn, 0));
// CHECK((x, up,   SP,up, SP,dn, SP,up, x,dn, x,up, 0));
// CHECK((j, dn,   SP,up, SP,dn, SP,up, x,dn, x,up, j,dn, 0));
// CHECK((j, up,   SP,up, SP,dn, SP,up, x,dn, x,up, j,dn, j,up, 0));

// overlapped slightly
// resetOutput();
// CHECK((SP, dn,  0));
// CHECK((x, dn,   SP,dn, x,dn, 0));
// CHECK((SP, up,  SP,dn, x,dn, SP,up, 0));
// CHECK((x, up,   SP,dn, x,dn, SP,up, x,up, 0));

//... plus repeating spaces
// CHECK((SP, dn,  0));
// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((SP, dn,  0));
// CHECK((SP, up,  SP,dn, j,dn, SP,up, 0));
// CHECK((j, up,   SP,dn, j,dn, SP,up, j,up, 0));

// key ups in waitMappedDown
// CHECK((SP, dn,  0));
// CHECK((x, up,   x,up, 0));
// CHECK((j, up,   x,up, j,up, 0));
// CHECK((SP, up,  x,up, j,up, SP,dn, SP,up, 0));

// other keys in waitMappedUp
// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((x, up,   SP,dn, j,dn, x,up, 0));
// CHECK((x, dn,   SP,dn, j,dn, x,up, x,dn, 0));
// CHECK((j, dn,   SP,dn, j,dn, x,up, x,dn, j,dn, 0));
// CHECK((SP, up,  SP,dn, j,dn, x,up, x,dn, j,dn, SP,up, 0));

// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((x, dn,   SP,dn, j,dn, x,dn, 0));
// CHECK((SP, up,  SP,dn, j,dn, x,dn, SP,up, 0));

// activate mapping
// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((j, up,   LE,edn, LE,up, 0));
// CHECK((SP, up,  LE,edn, LE,up, 0));

// autorepeat into mapping, and out
// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((j, dn,   LE,edn, LE,edn, 0));
// CHECK((j, dn,   LE,edn, LE,edn, LE,edn, 0));
// CHECK((j, up,   LE,edn, LE,edn, LE,edn, LE,up, 0));
// CHECK((SP, dn,  LE,edn, LE,edn, LE,edn, LE,up, 0));
// CHECK((j, dn,   LE,edn, LE,edn, LE,edn, LE,up, LE,edn, 0));
// CHECK((SP, up,  LE,edn, LE,edn, LE,edn, LE,up, LE,edn, LE,up, 0));
// CHECK((j, dn,   LE,edn, LE,edn, LE,edn, LE,up, LE,edn, LE,up, j,dn, 0));
// CHECK((j, up,   LE,edn, LE,edn, LE,edn, LE,up, LE,edn, LE,up, j,dn, j,up, 0));

// other keys during mapping
// CHECK((SP, dn,  0));
// CHECK((j, dn,   0));
// CHECK((j, up,   LE,edn, LE,up, 0));
// CHECK((x, dn,   LE,edn, LE,up, x,dn, 0));
// CHECK((x, up,   LE,edn, LE,up, x,dn, x,up, 0));
// CHECK((j, dn,   LE,edn, LE,up, x,dn, x,up, LE,edn, 0));
// CHECK((SP, up,  LE,edn, LE,up, x,dn, x,up, LE,edn, LE,up, 0));

// check space-emmitted states
// CHECK((SP, dn,  0));
// CHECK((x, dn,   SP,dn, x,dn, 0));
// CHECK((SP, dn,  SP,dn, x,dn, 0));
// CHECK((x, dn,   SP,dn, x,dn, x,dn, 0));
// CHECK((x, up,   SP,dn, x,dn, x,dn, x,up, 0));
// CHECK((j, up,   SP,dn, x,dn, x,dn, x,up, j,up, 0));
// CHECK((j, dn,   SP,dn, x,dn, x,dn, x,up, j,up, 0));
// CHECK((j, up,   SP,dn, x,dn, x,dn, x,up, j,up, LE,edn, LE,up, 0));
// CHECK((SP, up,  SP,dn, x,dn, x,dn, x,up, j,up, LE,edn, LE,up, 0)); //XXX should this emit a space (needs mappingSpaceEmitted state)

// wmuse
// CHECK((SP, dn,  0));
// CHECK((x, dn,   SP,dn, x,dn, 0));
// CHECK((j, dn,   SP,dn, x,dn, 0));
// CHECK((SP, dn,  SP,dn, x,dn, 0));
// CHECK((SP, up,  SP,dn, x,dn, j,dn, SP,up, 0));

// CHECK((SP, dn,  0));
// CHECK((x, dn,   SP,dn, x,dn, 0));
// CHECK((j, dn,   SP,dn, x,dn, 0));
// CHECK((j, dn,   SP,dn, x,dn, LE,edn, LE,edn, 0));
// CHECK((SP, up,  SP,dn, x,dn, LE,edn, LE,edn, LE,up, 0)); //XXX should this emit a space (needs mappingSpaceEmitted state)

// CHECK((SP, dn,  0));
// CHECK((x, dn,   SP,dn, x,dn, 0));
// CHECK((j, dn,   SP,dn, x,dn, 0));
// CHECK((x, up,   SP,dn, x,dn, x,up, 0));
// CHECK((j, up,   SP,dn, x,dn, x,up, LE,edn, LE,up, 0));
// CHECK((SP, up,  SP,dn, x,dn, x,up, LE,edn, LE,up, 0)); //XXX should this emit a space (needs mappingSpaceEmitted state)

// run configure tests
// idle
// CHECK((F5, dn,  F5,dn, 0));
// CHECK((SP, dn,  F5,dn, 0));
// wmd
// CHECK((F5, dn,  F5,dn, '*',dn, 0));
// CHECK((F5, up,  F5,dn, '*',dn, F5,up, 0));
// CHECK((j, dn,   F5,dn, '*',dn, F5,up, 0));
// wmu
// CHECK((F5, dn,  F5,dn, '*',dn, F5,up, '*',dn, 0));
// CHECK((j, up,   F5,dn, '*',dn, F5,up, '*',dn, LE,edn, LE,up, 0));
// mapping
// CHECK((F5, dn,  F5,dn, '*',dn, F5,up, '*',dn, LE,edn, LE,up, '*',dn, 0));
// CHECK((SP, up,  F5,dn, '*',dn, F5,up, '*',dn, LE,edn, LE,up, '*',dn, 0));

// CHECK((SP, dn,  0));
// wmd
// CHECK((x, dn,   SP,dn, x,dn, 0));
// wmd-se
// CHECK((F5, dn,  SP,dn, x,dn, '*',dn, 0));
// CHECK((j, dn,   SP,dn, x,dn, '*',dn, 0));
// wmu-se
// CHECK((F5, dn,  SP,dn, x,dn, '*',dn, '*',dn, 0));
// CHECK((SP, up,  SP,dn, x,dn, '*',dn, '*',dn, j,dn, SP,up, 0));

// Overlapping mapped keys
// CHECK((SP, dn,  0));
// CHECK((m, dn,   0));
// CHECK((j, dn,   DEL,edn, LE,edn, 0));
// CHECK((j, up,   DEL,edn, LE,edn, LE,up, 0));
// CHECK((m, up,   DEL,edn, LE,edn, LE,up, DEL,up, 0));
// CHECK((SP, up,  DEL,edn, LE,edn, LE,up, DEL,up, 0));

// Overlapping mapped keys -- space up first.
// should release held mapped keys.  (Fixes sticky Shift bug.)
// CHECK((SP, dn,  0));
// CHECK((m, dn,   0));
// CHECK((j, dn,   DEL,edn, LE,edn, 0));
// release order is in vk code order
// CHECK((SP, up,  DEL,edn, LE,edn, LE,up, DEL,up, 0));

// mapped modifier keys
// options.keyMapping['C'] = ctrlFlag | 'C'; // ctrl+c
// CHECK((SP, dn,  0));
// CHECK((c, dn,   0));
// CHECK((c, up,   ctrl,dn, c,dn, ctrl,up, c,up, 0));
// CHECK((c, dn,   ctrl,dn, c,dn, ctrl,up, c,up, ctrl,dn, c,dn, ctrl,up, 0));
// CHECK((c, up,   ctrl,dn, c,dn, ctrl,up, c,up, ctrl,dn, c,dn, ctrl,up, c,up, 0));
// CHECK((SP, up,  ctrl,dn, c,dn, ctrl,up, c,up, ctrl,dn, c,dn, ctrl,up, c,up, 0));
// with modifier already down:
// CHECK((SP, dn,  0));
// CHECK((ctrl,dn, ctrl,dn, 0));
// CHECK((c, dn,   ctrl,dn, 0));
// CHECK((c, up,   ctrl,dn, c,dn, c,up, 0));
// CHECK((ctrl,up, ctrl,dn, c,dn, c,up, ctrl,up, 0));
// CHECK((SP,up,   ctrl,dn, c,dn, c,up, ctrl,up, 0));

// training mode
// options.trainingMode = true;
// options.beepForMistakes = false;
// CHECK((x, dn,   x,dn, 0));
// CHECK((x, up,   x,dn, x,up, 0));
// CHECK((LE, edn, x,dn, x,up, 0));
// CHECK((LE, up,  x,dn, x,up, 0));
// with modifier mapping
// CHECK((c, dn,    c,dn, 0));
// CHECK((c, up,    c,dn, c,up, 0));
// CHECK((ctrl, dn, c,dn, c,up, ctrl,dn, 0));
// CHECK((c, dn,    c,dn, c,up, ctrl,dn, 0));
// CHECK((c, up,    c,dn, c,up, ctrl,dn, 0));
// CHECK((ctrl, up, c,dn, c,up, ctrl,dn, ctrl,up, 0));

// SM.printUnusedTransitions();
