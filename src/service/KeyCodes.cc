#include "KeyCodes.h"

int KeyCodes::getKeyCodeFromKeyString(const std::string &keyString) {
    auto keyCode = this->key_codes.find(keyString);
    if (keyCode != this->key_codes.end()) {
        return keyCode->second;
    } else {
        return 0;
    }
}
