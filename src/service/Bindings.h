//
// Created by adda on 14.6.22.
//

#ifndef TOUCHCURSOR_LINUX_HYPERBINDING_H
#define TOUCHCURSOR_LINUX_HYPERBINDING_H

#include <unordered_map>
#include <string>

class OriginalKeyNotFoundException: public std::exception {
    [[nodiscard]] const char* what() const noexcept override {
        return "Original key not found in bindings.";
    }
};

class HyperKeyNotFoundException: public std::exception {
    [[nodiscard]] const char* what() const noexcept override {
        return "Hyper key for the given hyper key name not found in hyper key names map.";
    }
};

using THyperKey = int;
using TOriginalKey = int;
using TMappedKey = int;
using TPressedKey = int;
using THyperName = std::string;
using TKeyBindingMap = std::unordered_map<TOriginalKey, TMappedKey>;
using THyperKeysMap = std::unordered_map<THyperKey, TKeyBindingMap>;
using THyperNamesMap = std::unordered_map<THyperName, THyperKey>;

class Bindings {
public:
    TKeyBindingMap& addHyperKey(const THyperKey& hyperKey);
    void addHyperName(const THyperName& hyperName, const THyperKey& hyperKey);
    THyperKey getHyperKeyForHyperName(const THyperName& hyperName);
    void addPermanentRemapping(const TOriginalKey& originalKey, const TMappedKey& mappedKey);
    void addCommonHyperMapping(const TOriginalKey& originalKey, const TMappedKey& mappedKey);
    void addHyperMapping(const THyperKey& hyperKey, const TOriginalKey& originalKey, const TMappedKey& mappedKey);
    bool hyperKeyExists(const THyperKey& hyperKey);
    void resolveKeyPress(const TPressedKey& pressedKey);
    TMappedKey& getMappedKeyForHyperBinding(const THyperKey& hyperKey, const TOriginalKey& originalKey);
    TMappedKey& getMappedKeyForPermanentRemapping(const TOriginalKey& originalKey);

    bool isMappedKeyForHyperBinding(THyperKey hyperKey, TOriginalKey originalKey);

private:
    TKeyBindingMap commonHyperBindings;
    THyperKeysMap hyperBindings;
    TKeyBindingMap permanentRemappings;
    THyperNamesMap hyperNamesMap;
};

#endif //TOUCHCURSOR_LINUX_HYPERBINDING_H
