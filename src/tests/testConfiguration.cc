#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <filesystem>

#include "../service/Bindings.h"
#include "../service/KeyCodes.h"
#include "../service/config.h"

TEST_CASE("Parse configuration file", "[config]") {
    KeyCodes keyCodes = KeyCodes{};
    std::filesystem::path configPath = std::filesystem::current_path() / "src/tests/config_files/simple_config.conf";
    THyperKey hyperKey{ keyCodes.getKeyCodeFromKeyString("KEY_SPACE") };
    TOriginalKey originalKey{ keyCodes.getKeyCodeFromKeyString("KEY_H") };
    TMappedKey mappedKey{ keyCodes.getKeyCodeFromKeyString("KEY_G") };

    auto bindings = readConfiguration(std::string{configPath});

    REQUIRE(bindings.hyperKeyExists(hyperKey));
    REQUIRE(bindings.getMappedKeyForHyperBinding(hyperKey, originalKey) == mappedKey);
}