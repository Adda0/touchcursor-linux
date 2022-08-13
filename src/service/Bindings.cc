#include "Bindings.h"

void Bindings::addHyperMapping(const THyperKey &hyperKey, const TOriginalKey &originalKey,
                               const TMappedKeysSequenceList &mappedKeysSequenceList) {
    auto &keyBindingMap = this->addHyperKey(hyperKey);

    keyBindingMap.insert_or_assign(originalKey, mappedKeysSequenceList);
    //std::cout << "After insertion: " << keyBindingMap.find(originalKey)->second << "\n";
    //std::cout << "With method: " << this->getMappedKeyForHyperBinding(hyperKey, originalKey) << "\n";
}

bool Bindings::hyperKeyExists(const THyperKey &hyperKey) {
    return this->hyperBindings.find(hyperKey) != this->hyperBindings.end();
}

void Bindings::addPermanentRemapping(const TOriginalKey &originalKey, const TMappedKeysSequenceList &mappedKeysSequenceList) {
    this->permanentRemappings.insert_or_assign(originalKey, mappedKeysSequenceList);
}

TMappedKeysSequenceList& Bindings::getMappedKeyForHyperBinding(const THyperKey &hyperKey, const TOriginalKey &originalKey) {
    auto &hyperKeyBindings = this->hyperBindings.find(hyperKey)->second;
    //std::cout << "Empty: " << hyperKeyBindings.empty() << "\n";
    auto originalKeyBinding = hyperKeyBindings.find(originalKey);
    if (originalKeyBinding == hyperKeyBindings.end()) {
        originalKeyBinding = this->commonHyperBindings.find(originalKey);
    }

    return originalKeyBinding->second;
}

TMappedKeysSequenceList& Bindings::getMappedKeyForPermanentRemapping(const TOriginalKey &originalKey) {
    auto permanentRemapping = this->permanentRemappings.find(originalKey);
    if (permanentRemapping != this->permanentRemappings.end()) {
        return permanentRemapping->second;
    }
}

void Bindings::addCommonHyperMapping(const TOriginalKey &originalKey, const TMappedKeysSequenceList& mappedKeysSequenceList) {
    this->commonHyperBindings.insert_or_assign(originalKey, mappedKeysSequenceList);
}

TKeyBindingMap &Bindings::addHyperKey(const THyperKey &hyperKey) {
    auto hyperKeyMap{this->hyperBindings.find(hyperKey)};
    if (hyperKeyMap != this->hyperBindings.end()) {
        return hyperKeyMap->second;
    }

    return this->hyperBindings.insert({hyperKey, TKeyBindingMap{}}).first->second;
}

void Bindings::addHyperName(const THyperName &hyperName, const THyperKey &hyperKey) {
    this->hyperNamesMap.insert_or_assign(hyperName, hyperKey);
}

THyperKey Bindings::addHyperNameWithoutHyperKey(const THyperName &hyperName) {
    this->hyperNamesWithoutKey.push_back(hyperName);
    return std::hash<std::string>{}(hyperName);
}

THyperKey Bindings::getHyperKeyForHyperName(const THyperName &hyperName) {
    auto hyperKey{this->hyperNamesMap.find(hyperName)};
    if (hyperKey != this->hyperNamesMap.end()) {
        return hyperKey->second;
    }

    return this->addHyperNameWithoutHyperKey(hyperName);
}

bool Bindings::isMappedKeyForHyperBinding(THyperKey hyperKey, TOriginalKey originalKey) {
    auto hyperKeyBindings{this->hyperBindings.find(hyperKey)->second};

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

bool Bindings::isMappedKeyForPermanentRemapping(const TOriginalKey &originalKey) {
    return this->permanentRemappings.find(originalKey) != this->permanentRemappings.end();
}

bool Bindings::isHyperNamesWithoutKeyEmpty() {
    return this->hyperNamesWithoutKey.empty();
}

void Bindings::bindHyperNamesWithoutKeys() {
    for (const auto& hyperNameWithoutKey: hyperNamesWithoutKey) {
        auto hyperNameKey{hyperNamesMap.find(hyperNameWithoutKey)};
        if (hyperNameKey != hyperNamesMap.end()) {
            THyperKey defaultHyperNameKey = std::hash<std::string>{}(hyperNameWithoutKey);
            auto hyperBindingsForKey{hyperBindings.find(defaultHyperNameKey)};
            if (hyperBindingsForKey != hyperBindings.end()) {
                auto hyperBindingsStruct{hyperBindings.extract(defaultHyperNameKey).mapped()};
                this->hyperBindings.insert_or_assign(hyperNameKey->second, hyperBindingsStruct);
            }
        } else {
            throw HyperNameWithoutKeyException();
        }
    }

    this->hyperNamesWithoutKey.clear();
}

bool Bindings::isSequenceHyperMapping(THyperKey hyperKey, TOriginalKey originalKey) {
    auto hyperKeyBindings{this->hyperBindings.find(hyperKey)->second};

    if (hyperKeyBindings.find(originalKey) == hyperKeyBindings.end()) {
        auto commonHyperKeyBindings = this->commonHyperBindings.find(originalKey);
        if (commonHyperKeyBindings == commonHyperBindings.end()) {
            return false;
        }

        if (commonHyperKeyBindings->second.size() > 1) {
            return true;
        }

        return false;
    }

    if (hyperKeyBindings.size() > 1) {
        return true;
    }

    return false;
}
