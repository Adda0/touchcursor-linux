#include <catch2/catch_test_macros.hpp>

#include "../service/KeyCodes.h"

TEST_CASE("Get key codes", "[keyCodes]") {
    KeyCodes keyCodes = KeyCodes{};

    SECTION("Key string does not exist as a key in the hash table.") {
        REQUIRE(keyCodes.getKeyCodeFromKeyString("KEY_123") == 0);
    }

    SECTION("Key string does not exist as a key in the hash table.") {
        REQUIRE(keyCodes.getKeyCodeFromKeyString("KEY_ESC") == 1);
        REQUIRE(keyCodes.getKeyCodeFromKeyString("KEY_X") == 45);
        REQUIRE(keyCodes.getKeyCodeFromKeyString("KEY_Y") == 21);
        REQUIRE(keyCodes.getKeyCodeFromKeyString("KEY_ROTATE_LOCK_TOGGLE") == 0x231);

    }
}