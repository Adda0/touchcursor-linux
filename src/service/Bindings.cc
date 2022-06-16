//
// Created by adda on 14.6.22.
//

#include <iostream>

#include "Bindings.h"

void Bindings::addHyperMapping(const THyperKey& hyperKey, const TOriginalKey& originalKey, const TMappedKey& mappedKey) {
    auto& keyBindingMap = this->addHyperKey(hyperKey);

    keyBindingMap.insert_or_assign(originalKey, mappedKey);
    std::cout << "After insertion: " << keyBindingMap.find(originalKey)->second << "\n";

    std::cout << "With method: " << this->getMappedKeyForHyperBinding(hyperKey, originalKey) << "\n";
}

bool Bindings::hyperKeyExists(const THyperKey& hyperKey) {
    return this->hyperBindings.find(hyperKey) != this->hyperBindings.end();
}

void Bindings::addPermanentRemapping(const TOriginalKey& originalKey, const TMappedKey& mappedKey) {
    this->permanentRemappings.insert_or_assign(originalKey, mappedKey);
}

void Bindings::resolveKeyPress(const TPressedKey& pressedKey) {
    // TODO.
}

TMappedKey &Bindings::getMappedKeyForHyperBinding(const THyperKey& hyperKey, const TOriginalKey& originalKey) {
    auto& hyperKeyBindings = this->hyperBindings.find(hyperKey)->second;
    std::cout << "Empty: " << hyperKeyBindings.empty() << "\n";
    auto originalKeyBinding = hyperKeyBindings.find(originalKey);
    if (originalKeyBinding == hyperKeyBindings.end()) {
        throw OriginalKeyNotFoundException();
    }

    return originalKeyBinding->second;
}

TMappedKey &Bindings::getMappedKeyForPermanentRemapping(const TOriginalKey &originalKey) {
    auto permanentRemapping = this->permanentRemappings.find(originalKey);
    if (permanentRemapping != this->permanentRemappings.end()) {
        return permanentRemapping->second;
    }

    throw OriginalKeyNotFoundException();
}

void Bindings::addCommonHyperMapping(const TOriginalKey &originalKey, const TMappedKey &mappedKey) {
    this->commonHyperBindings.insert_or_assign(originalKey, mappedKey);
}

TKeyBindingMap& Bindings::addHyperKey(const THyperKey &hyperKey) {
    return this->hyperBindings.insert({hyperKey, TKeyBindingMap{}}).first->second;
}
