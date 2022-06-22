#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <linux/input.h>

#include <catch2/catch_test_macros.hpp>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/config.h"
#include "../service/mapper.h"

// minunit http://www.jera.com/techinfo/jtns/jtn002.html
#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { int result = test(); tests_run++; if (result != 0) return result; } while (0)

extern char outputString[256];

/*
 * Simulates typing keys.
 * The method arguments should be number of arguments, then pairs of key code and key value.
 */
static void type(Bindings& bindings, int num, ...)
{
    for(int i = 0; i < 256; i++) outputString[i] = 0;
    va_list arguments;
    va_start(arguments, num);
    for (int i = 0; i < num; i += 2)
    {
        int code = va_arg(arguments, int);
        int value = va_arg(arguments, int);
        processKey(bindings, EV_KEY, code, value);
    }
    va_end(arguments);
}

/*
 * Tests for normal (slow) typing.
 * These tests should rarely have overlapping key events.
 */
//static int testNormalTyping()
//{
//
//    // Space down, up, down, up
//    description = "sd, su, sd, su";
//    expected    = "57:1 57:0 57:1 57:0 ";
//    type(8, KEY_SPACE, 1, KEY_SPACE, 0, KEY_SPACE, 1, KEY_SPACE, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Other down, Space down, up, Other up
//    description = "od, sd, su, ou";
//    expected    = "31:1 57:1 57:0 31:0 ";
//    type(8, KEY_S, 1, KEY_SPACE, 1, KEY_SPACE, 0, KEY_S, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Space down, Other down, up, Space up
//    description = "sd, od, ou, su";
//    expected    = "57:1 31:1 31:0 57:0 ";
//    type(8, KEY_SPACE, 1, KEY_S, 1, KEY_S, 0, KEY_SPACE, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Mapped down, Space down, up, Mapped up
//    description = "md, sd, su, mu";
//    expected    = "36:1 57:1 57:0 36:0 ";
//    type(8, KEY_J, 1, KEY_SPACE, 1, KEY_SPACE, 0, KEY_J, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Space down, Mapped down, up, Space up
//    description = "sd, md, mu, su";
//    expected    = "105:1 105:0 ";
//    type(8, KEY_SPACE, 1, KEY_J, 1, KEY_J, 0, KEY_SPACE, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Mapped down, Space down, up, Different mapped down, up, Mapped up
//    description = "m1d, sd, su, m2d, m2u, m1u";
//    expected    = "36:1 57:1 57:0 23:1 23:0 36:0 ";
//    type(12, KEY_J, 1, KEY_SPACE, 1, KEY_SPACE, 0, KEY_I, 1, KEY_I, 0, KEY_J, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    return 0;
//}
//
///*
// * Tests for fast typing.
// * These tests should have many overlapping key events.
// */
//static int testFastTyping()
//{
//    // Space down, mapped down, space up, mapped up
//    // The mapped key should not be converted
//    char* description = "sd, md, su, mu";
//    char* expected    = "57:1 36:1 57:0 36:0 ";
//    type(8, KEY_SPACE, 1, KEY_J, 1, KEY_SPACE, 0, KEY_J, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Mapped down, space down, mapped up, space up
//    // The mapped key should not be converted
//    // This is not out of order, remember space down does not emit anything
//    description = "md, sd, mu, su";
//    expected    = "36:1 36:0 57:1 57:0 ";
//    type(8, KEY_J, 1, KEY_SPACE, 1, KEY_J, 0, KEY_SPACE, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Space down, mapped1 down, mapped2 down, space up, mapped1 up, mapped2 up
//    // The mapped keys should be sent converted
//    // Extra up events are sent, but that does not matter
//    description = "sd, m1d, m2d, su, m1u, m2u";
//    expected    = "105:1 103:1 105:0 103:0 36:0 23:0 ";
//    type(12, KEY_SPACE, 1, KEY_J, 1, KEY_I, 1, KEY_SPACE, 0, KEY_J, 0, KEY_I, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    // Space down, mapped1 down, mapped2 down, mapped3 down, space up, mapped1 up, mapped2 up, mapped3 up
//    // The mapped keys should be sent converted
//    // Extra up events are sent, but that does not matter
//    description = "sd, m1d, m2d, m3d, su, m1u, m2u, m3u";
//    expected    = "105:1 103:1 106:1 105:0 103:0 106:0 36:0 23:0 38:0 ";
//    type(16, KEY_SPACE, 1, KEY_J, 1, KEY_I, 1, KEY_L, 1, KEY_SPACE, 0, KEY_J, 0, KEY_I, 0, KEY_L, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    return 0;
//}
//
///*
// * Tests for fast typing.
// * These tests should have many overlapping key events.
// */
//static int testSpecialTyping()
//{
//    // Space down, other (modifier) down, other (modifier) up, space up
//    // The key should be output, hyper mode retained
//    char* description = "sd, od, ou, su";
//    char* expected    = "42:1 42:0 ";
//    type(6, KEY_SPACE, 1, KEY_LEFTSHIFT, 1, KEY_LEFTSHIFT, 0, KEY_SPACE, 0);
//    if (strcmp(expected, output) != 0) {
//        printf("[%s] failed. expected: '%s', output: '%s'\n", description, expected, output);
//        return 1;
//    }
//    else {
//        printf("[%s] passed. expected: '%s', output: '%s'\n", description, expected, output);
//    }
//
//    return 0;
//}
//
///*
// * Simple method for running all tests.
// */
//static int runTests()
//{
//    // default config
//    hyperKey = KEY_SPACE;
//    keymap[KEY_I] = KEY_UP;
//    keymap[KEY_J] = KEY_LEFT;
//    keymap[KEY_K] = KEY_DOWN;
//    keymap[KEY_L] = KEY_RIGHT;
//    keymap[KEY_H] = KEY_PAGEUP;
//    keymap[KEY_N] = KEY_PAGEDOWN;
//    keymap[KEY_U] = KEY_HOME;
//    keymap[KEY_O] = KEY_END;
//    keymap[KEY_M] = KEY_DELETE;
//    keymap[KEY_P] = KEY_BACKSPACE;
//    keymap[KEY_Y] = KEY_INSERT;
//
//    mu_run_test(testNormalTyping);
//    mu_run_test(testFastTyping);
//    mu_run_test(testSpecialTyping);
//}

TEST_CASE("Emit events according to configuration", "[emit]") {
    KeyCodes keyCodes = KeyCodes{};

    TOriginalKey originalKey{ keyCodes.getKeyCodeFromKeyString("KEY_H") };
    TMappedKey mappedKey{ keyCodes.getKeyCodeFromKeyString("KEY_G") };
    std::string expected{};

    SECTION("Emit according to config_files/simple_config_multiple_hyper_keys.conf") {
        THyperKey hyperKeySpace{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        THyperKey hyperKeyV{ keyCodes.getKeyCodeFromKeyString("KEY_V") };

        TOriginalKey originalKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_U") };
        TMappedKey mappedKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_T") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_multiple_hyper_keys.conf";

        auto bindings = readConfiguration(std::string{configPath});

        SECTION("Space down, up") {
            expected = "57:1 57:0 ";
            type(bindings, 4, KEY_SPACE, 1, KEY_SPACE, 0);
            REQUIRE(expected == outputString);
        }

        SECTION("V down, up") {
            expected = "47:1 47:0 ";
            type(bindings, 4, KEY_V, 1, KEY_V, 0);
            REQUIRE(expected == outputString);
        }

        SECTION("Space down, H down, H up, space up") {
            expected = "34:1 34:0 ";
            type(bindings, 8, KEY_SPACE, 1, KEY_H, 1, KEY_H, 0, KEY_SPACE, 0);
            REQUIRE(expected == outputString);
        }

        SECTION("Space down, Mapped down, up, Space up") {
            expected = "105:1 105:0 ";
            type(bindings, 8, KEY_SPACE, 1, KEY_J, 1, KEY_J, 0, KEY_SPACE, 0);
            REQUIRE(expected == outputString);
        }

        SECTION("V down, Mapped down, up, V up") {
            expected = "105:1 105:0 ";
            type(bindings, 8, KEY_V, 1, KEY_J, 1, KEY_J, 0, KEY_V, 0);
            REQUIRE(expected == outputString);
        }

        // TODO: Correctly set what to expect.
        //SECTION("V down, Space down, Space up, V up") {
        //    expected = "105:1 105:0 ";
        //    type(bindings, 8, KEY_V, 1, KEY_SPACE, 1, KEY_SPACE, 0, KEY_V, 0);
        //    REQUIRE(expected == outputString);
        //}
    }
}