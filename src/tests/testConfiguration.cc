#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <filesystem>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/config.h"

TEST_CASE("Parse simple configuration file", "[config]") {
    KeyCodes keyCodes = KeyCodes{};

    TOriginalKey originalKey{ keyCodes.getKeyCodeFromKeyString("KEY_H") };
    TMappedKey mappedKey{ keyCodes.getKeyCodeFromKeyString("KEY_G") };

    SECTION("Parse config_files/simple_config.conf") {
        THyperKey hyperKey{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config.conf";

        auto bindings = readConfiguration(std::string{configPath});

        REQUIRE(bindings.hyperKeyExists(hyperKey));
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKey, originalKey) == mappedKey);
    }

    SECTION("Parse config_files/simple_config_multiple_hyper_keys.conf") {
        THyperKey hyperKeySpace{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        THyperKey hyperKeyV{ keyCodes.getKeyCodeFromKeyString("KEY_V") };

        TOriginalKey originalKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_U") };
        TMappedKey mappedKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_T") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_multiple_hyper_keys.conf";

        auto bindings = readConfiguration(std::string{configPath});

        REQUIRE(bindings.hyperKeyExists(hyperKeySpace));
        REQUIRE(bindings.hyperKeyExists(hyperKeyV));
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == mappedKey);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKey) == mappedKey);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific) == mappedKeySpecific);
        
    }
}