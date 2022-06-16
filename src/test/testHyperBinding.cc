#include <catch2/catch_test_macros.hpp>

#include "../service/Bindings.h"

TEST_CASE("Insert hyper key bindings", "[hyperKey]") {
    Bindings bindings = Bindings{};
    auto hyperKey = "KEY_X";
    auto originalKey = "KEY_M";
    auto mappedKey = "KEY_E";

    bindings.addHyperKey(hyperKey);
    REQUIRE(bindings.hyperKeyExists(hyperKey));
}
