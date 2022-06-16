#include <catch2/catch_test_macros.hpp>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"

TEST_CASE("Access bindings map", "[bindings]") {
    Bindings bindings = Bindings{};
    KeyCodes keyCodes = KeyCodes{};
    auto originalKey = keyCodes.getKeyCodeFromKeyString("KEY_M");
    auto mappedKey = keyCodes.getKeyCodeFromKeyString("KEY_E");

    SECTION("Manipulate with hyper key bindings") {
        auto hyperKey = keyCodes.getKeyCodeFromKeyString("KEY_X");

        SECTION("Add hyper key explicitly") {
            bindings.addHyperKey(hyperKey);
            REQUIRE(bindings.hyperKeyExists(hyperKey));
        }

        bindings.addHyperMapping(hyperKey, originalKey, mappedKey);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKey, originalKey) == mappedKey);

        SECTION("Look for nonexistent original key for an existing hyper key") {
            REQUIRE_THROWS_AS(bindings.getMappedKeyForHyperBinding(hyperKey, 0), OriginalKeyNotFoundException);
        }
    }

    SECTION("Manipulate with permanent remappings") {
        bindings.addPermanentRemapping(originalKey, mappedKey);
        REQUIRE(bindings.getMappedKeyForPermanentRemapping(originalKey) == mappedKey);

        SECTION("Look for nonexistent original key in permanent remappings") {
            REQUIRE_THROWS_AS(bindings.getMappedKeyForPermanentRemapping(0), OriginalKeyNotFoundException);
        }
    }
}