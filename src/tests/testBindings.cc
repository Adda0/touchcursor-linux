#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/Config.h"

TEST_CASE("Access bindings map", "[bindings]") {
    Config config{};
    KeyCodes keyCodes{};
    auto originalKey = keyCodes.getKeyCodeFromKeyString("KEY_M");
    auto mappedKey = keyCodes.getKeyCodeFromKeyString("KEY_E");

    SECTION("Manipulate with hyper key bindings") {
        auto hyperKey = keyCodes.getKeyCodeFromKeyString("KEY_X");
        //std::cout << "Counter: \n";

        SECTION("Add hyper key explicitly") {
            config.bindings.addHyperKey(hyperKey);
            REQUIRE(config.bindings.hyperKeyExists(hyperKey));
        }

        SECTION("Do not add hyper key explicitly") {
            REQUIRE(!config.bindings.hyperKeyExists(hyperKey));
        }

        config.bindings.addHyperMapping(hyperKey, originalKey, TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKey, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));

        // TODO: Decide what to do when looking for an nonexistent key.
        //SECTION("Look for nonexistent original key for an existing hyper key") {
        //    REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKey, 0) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, 0)));
        //}
    }

    SECTION("Manipulate with permanent remappings") {
        config.bindings.addPermanentRemapping(originalKey, TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForPermanentRemapping(originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));

        //SECTION("Look for nonexistent original key in permanent remappings") {
        //    REQUIRE_THROWS_AS(config.bindings.getMappedKeyForPermanentRemapping(0), OriginalKeyNotFoundException);
        //}
    }
}