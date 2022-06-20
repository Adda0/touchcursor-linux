#include <iostream>
#include <filesystem>

#include <catch2/catch_test_macros.hpp>

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
        TOriginalKey originalKeyJ{ keyCodes.getKeyCodeFromKeyString("KEY_J") };
        TMappedKey mappedKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_T") };
        TMappedKey mappedKeySpecificSpace{ keyCodes.getKeyCodeFromKeyString("KEY_P") };
        TMappedKey mappedKeyJ{ keyCodes.getKeyCodeFromKeyString("KEY_LEFT") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_multiple_hyper_keys.conf";

        auto bindings = readConfiguration(std::string{configPath});

        REQUIRE(bindings.hyperKeyExists(hyperKeySpace));
        REQUIRE(bindings.hyperKeyExists(hyperKeyV));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKey));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKey));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeyJ));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific));
        REQUIRE(bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific));
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == mappedKey);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKey) == mappedKey);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific) == mappedKeySpecific);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific) == mappedKeySpecificSpace);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ) == mappedKeyJ);
        REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeyJ) == mappedKeyJ);
    }

    SECTION("Parse config_files/simple_config_only_specific_hyper_key.conf") {
        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_only_specific_hyper_key.conf";

        REQUIRE_THROWS_AS(readConfiguration(std::string{configPath}), HyperKeyNotFoundException);
    }

    SECTION("Parse config_files/simple_config_remap.conf") {
        TOriginalKey originalKeyRemap{ keyCodes.getKeyCodeFromKeyString("KEY_T") };
        TMappedKey mappedKeyRemap{ keyCodes.getKeyCodeFromKeyString("KEY_M") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_remap.conf";

        auto bindings = readConfiguration(std::string{configPath});

        REQUIRE(bindings.getMappedKeyForPermanentRemapping(originalKeyRemap) == mappedKeyRemap);
    }
}