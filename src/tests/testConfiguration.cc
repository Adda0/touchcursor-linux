#include <iostream>
#include <filesystem>

#include <catch2/catch_test_macros.hpp>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/Config.h"

TEST_CASE("Parse simple configuration file", "[config]") {
    KeyCodes keyCodes = KeyCodes{};

    TOriginalKey originalKey{ keyCodes.getKeyCodeFromKeyString("KEY_H") };
    TMappedKey mappedKey{ keyCodes.getKeyCodeFromKeyString("KEY_G") };

    SECTION("Parse config_files/simple_config.conf") {
        THyperKey hyperKey{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config.conf";

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.hyperKeyExists(hyperKey));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKey, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
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

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.hyperKeyExists(hyperKeySpace));
        REQUIRE(config.bindings.hyperKeyExists(hyperKeyV));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKey));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKey));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeyJ));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeySpecific)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeySpecificSpace)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeyJ)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeyJ) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeyJ)));
    }

    SECTION("Parse config_files/simple_config_only_specific_hyper_key.conf") {
        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config_only_specific_hyper_key.conf";

        REQUIRE_THROWS_AS(Config::fromConfigFile(std::string{configPath}), HyperNameWithoutKeyException);
    }

    SECTION("Parse config_files/simple_config_remap.conf") {
        TOriginalKey originalKeyRemap{ keyCodes.getKeyCodeFromKeyString("KEY_T") };
        TMappedKey mappedKeyRemap{ keyCodes.getKeyCodeFromKeyString("KEY_M") };

        std::filesystem::path configPath{ std::filesystem::current_path() / "src/tests/config_files/simple_config_remap.conf" };

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.getMappedKeyForPermanentRemapping(originalKeyRemap) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeyRemap)));
    }

    SECTION("Parse config_files/config_reordered_tables.conf") {
        THyperKey hyperKeySpace{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        THyperKey hyperKeyV{ keyCodes.getKeyCodeFromKeyString("KEY_V") };

        TOriginalKey originalKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_U") };
        TOriginalKey originalKeyJ{ keyCodes.getKeyCodeFromKeyString("KEY_J") };
        TMappedKey mappedKeySpecific{ keyCodes.getKeyCodeFromKeyString("KEY_T") };
        TMappedKey mappedKeySpecificSpace{ keyCodes.getKeyCodeFromKeyString("KEY_P") };
        TMappedKey mappedKeyJ{ keyCodes.getKeyCodeFromKeyString("KEY_LEFT") };

        std::filesystem::path configPath{ std::filesystem::current_path() / "src/tests/config_files/config_reordered_tables.conf" };

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.hyperKeyExists(hyperKeySpace));
        REQUIRE(config.bindings.hyperKeyExists(hyperKeyV));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKey));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKey));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeyJ));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKey)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeySpecific) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeySpecific)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeySpecific) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeySpecificSpace)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeyJ)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKeyJ) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, mappedKeyJ)));
    }

    SECTION("Parse config_files/reassign_key_configuration.conf") {
        THyperKey hyperKeySpace{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
        THyperKey hyperKeyV{ keyCodes.getKeyCodeFromKeyString("KEY_V") };

        TMappedKey newMappedKeyForHyperSpace{ keyCodes.getKeyCodeFromKeyString("KEY_M") };
        TMappedKey newMappedKeyForHyperV{ keyCodes.getKeyCodeFromKeyString("KEY_T") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/reassign_key_configuration.conf";

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, newMappedKeyForHyperSpace)));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeyV, originalKey) == TMappedKeysSequenceList(1, TMappedKeysCombinationList(1, newMappedKeyForHyperV)));
    }

    SECTION("Parse config_files/sequence_combination.conf") {
        THyperKey hyperKeySpace{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };

        TOriginalKey originalKeyJ{ keyCodes.getKeyCodeFromKeyString("KEY_J") };
        TOriginalKey originalKeyI{ keyCodes.getKeyCodeFromKeyString("KEY_I") };

        std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/sequence_combination.conf";

        Config config{ Config::fromConfigFile(std::string{configPath}) };

        REQUIRE(config.bindings.hyperKeyExists(hyperKeySpace));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKey));
        REQUIRE(config.bindings.isMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ));

        TMappedKeysCombinationList originalKeyHBindings{};
        originalKeyHBindings.push_back(keyCodes.getKeyCodeFromKeyString("KEY_LEFTSHIFT"));
        originalKeyHBindings.push_back(keyCodes.getKeyCodeFromKeyString("KEY_G"));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKey) == TMappedKeysSequenceList(1, originalKeyHBindings));

        TMappedKeysSequenceList originalKeyJBindings{};
        originalKeyJBindings.push_back(TMappedKeysCombinationList(1, keyCodes.getKeyCodeFromKeyString("KEY_T")));
        originalKeyJBindings.push_back(TMappedKeysCombinationList(1, keyCodes.getKeyCodeFromKeyString("KEY_F")));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeyJ) == originalKeyJBindings);

        TMappedKeysSequenceList originalKeyIBindings{};
        originalKeyIBindings.push_back(TMappedKeysCombinationList(1, keyCodes.getKeyCodeFromKeyString("KEY_UP")));
        TMappedKeysCombinationList originalKeyICombinationBindings{};
        originalKeyICombinationBindings.push_back(keyCodes.getKeyCodeFromKeyString("KEY_LEFTSHIFT"));
        originalKeyICombinationBindings.push_back(keyCodes.getKeyCodeFromKeyString("KEY_G"));
        originalKeyIBindings.push_back(originalKeyICombinationBindings);
        originalKeyIBindings.push_back(TMappedKeysCombinationList(1, keyCodes.getKeyCodeFromKeyString("KEY_F")));
        REQUIRE(config.bindings.getMappedKeyForHyperBinding(hyperKeySpace, originalKeyI) == originalKeyIBindings);
    }
}