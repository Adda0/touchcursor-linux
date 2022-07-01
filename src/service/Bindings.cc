#include <iostream>

#include "Bindings.h"

void Bindings::addHyperMapping(const THyperKey& hyperKey, const TOriginalKey& originalKey, const TMappedKey& mappedKey) {
    auto& keyBindingMap = this->addHyperKey(hyperKey);

    keyBindingMap.insert_or_assign(originalKey, mappedKey);
    //std::cout << "After insertion: " << keyBindingMap.find(originalKey)->second << "\n";
    //std::cout << "With method: " << this->getMappedKeyForHyperBinding(hyperKey, originalKey) << "\n";
}

bool Bindings::hyperKeyExists(const THyperKey& hyperKey) {
    return this->hyperBindings.find(hyperKey) != this->hyperBindings.end();
}

void Bindings::addPermanentRemapping(const TOriginalKey& originalKey, const TMappedKey& mappedKey) {
    this->permanentRemappings.insert_or_assign(originalKey, mappedKey);
}

TMappedKey Bindings::getMappedKeyForHyperBinding(const THyperKey& hyperKey, const TOriginalKey& originalKey) {
    auto& hyperKeyBindings = this->hyperBindings.find(hyperKey)->second;
    //std::cout << "Empty: " << hyperKeyBindings.empty() << "\n";
    auto originalKeyBinding = hyperKeyBindings.find(originalKey);
    if (originalKeyBinding == hyperKeyBindings.end()) {
        originalKeyBinding = this->commonHyperBindings.find(originalKey);
        if (originalKeyBinding == this->commonHyperBindings.end()) {
            return originalKey;
        }
    }

    return originalKeyBinding->second;
}

TMappedKey Bindings::getMappedKeyForPermanentRemapping(const TOriginalKey &originalKey) {
    auto permanentRemapping = this->permanentRemappings.find(originalKey);
    if (permanentRemapping != this->permanentRemappings.end()) {
        return permanentRemapping->second;
    }

    return originalKey;
}

void Bindings::addCommonHyperMapping(const TOriginalKey &originalKey, const TMappedKey &mappedKey) {
    this->commonHyperBindings.insert_or_assign(originalKey, mappedKey);
}

TKeyBindingMap& Bindings::addHyperKey(const THyperKey &hyperKey) {
    auto hyperKeyMap{ this->hyperBindings.find(hyperKey) };
    if (hyperKeyMap != this->hyperBindings.end()) {
        return hyperKeyMap->second;
    }

    return this->hyperBindings.insert({hyperKey, TKeyBindingMap{}}).first->second;
}

void Bindings::addHyperName(const THyperName& hyperName, const THyperKey& hyperKey) {
    this->hyperNamesMap.insert_or_assign(hyperName, hyperKey);
}

THyperKey Bindings::addHyperNameWithoutHyperKey(const THyperName& hyperName) {
    this->hyperNamesWithoutKey.push_back(hyperName);
    return std::hash<std::string>{}(hyperName);
}

THyperKey Bindings::getHyperKeyForHyperName(const THyperName &hyperName) {
    auto hyperKey { this->hyperNamesMap.find(hyperName) };
    if (hyperKey != this->hyperNamesMap.end()) {
        return hyperKey->second;
    }

    return this->addHyperNameWithoutHyperKey(hyperName);
}

bool Bindings::isMappedKeyForHyperBinding(THyperKey hyperKey, TOriginalKey originalKey) {
    auto hyperKeyBindings{ this->hyperBindings.find(hyperKey)->second };

    if (hyperKeyBindings.find(originalKey) == hyperKeyBindings.end()) {
        if (hyperKey == originalKey) {
            return false;
        }

        auto commonHyperKeyBindings = this->commonHyperBindings.find(originalKey);
        if (commonHyperKeyBindings == commonHyperBindings.end()) {
            return false;
        }
    }

    return true;
}

bool Bindings::permanentRemappingExists(const TOriginalKey &originalKey) {
    return this->permanentRemappings.find(originalKey) != this->permanentRemappings.end();
}

bool Bindings::isHyperNamesWithoutKeyEmpty() {
    return this->hyperNamesWithoutKey.empty();
}

void Bindings::bindHyperNamesWithoutKeys() {
    for (auto hyperNameWithoutKey : this->hyperNamesWithoutKey) {
        auto hyperNameKey{ this->hyperNamesMap.find(hyperNameWithoutKey) };
        if (hyperNameKey != this->hyperNamesMap.end()) {
            THyperKey defaultHyperNameKey = std::hash<std::string>{}(hyperNameWithoutKey);
            auto hyperBindings{ this->hyperBindings.find(defaultHyperNameKey) };
            if (hyperBindings != this->hyperBindings.end()) {
                auto hyperBindingsStruct { this->hyperBindings.extract(defaultHyperNameKey).mapped() };
                this->hyperBindings.insert_or_assign(hyperNameKey->second, hyperBindingsStruct);
            }
        } else {
            throw HyperNameWithoutKeyException();
        }
    }

    this->hyperNamesWithoutKey.clear();
}
