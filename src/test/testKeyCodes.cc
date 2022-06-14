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

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
REQUIRE( Factorial(1) == 1 );
REQUIRE( Factorial(2) == 2 );
REQUIRE( Factorial(3) == 6 );
REQUIRE( Factorial(10) == 3628800 );
}