#ifndef BINDINGS_H
#define BINDINGS_H

#include <unordered_map>
#include <string>
#include <vector>

class HyperNameWithoutKeyException : public std::exception {
    [[nodiscard]] const char *what() const noexcept override {
        return "Hyper name used in table [Bindings] without a specified key mapped to the hyper name in table [Hyper].";
    }
};

using THyperKey = int;
using TOriginalKey = int;
using TMappedKey = int;
using THyperName = std::string;
using TKeyBindingMap = std::unordered_map<TOriginalKey, TMappedKey>;
using THyperKeysMap = std::unordered_map<THyperKey, TKeyBindingMap>;
using THyperNamesMap = std::unordered_map<THyperName, THyperKey>;
using THyperNamesWithoutKey = std::vector<THyperName>;

class Bindings {
public:
    TKeyBindingMap &addHyperKey(const THyperKey &hyperKey);

    void addHyperName(const THyperName &hyperName, const THyperKey &hyperKey);

    THyperKey addHyperNameWithoutHyperKey(const THyperName &hyperName);

    THyperKey getHyperKeyForHyperName(const THyperName &hyperName);

    void addPermanentRemapping(const TOriginalKey &originalKey, const TMappedKey &mappedKey);

    void addCommonHyperMapping(const TOriginalKey &originalKey, const TMappedKey &mappedKey);

    void addHyperMapping(const THyperKey &hyperKey, const TOriginalKey &originalKey, const TMappedKey &mappedKey);

    bool hyperKeyExists(const THyperKey &hyperKey);

    TMappedKey getMappedKeyForHyperBinding(const THyperKey &hyperKey, const TOriginalKey &originalKey);

    TMappedKey getMappedKeyForPermanentRemapping(const TOriginalKey &originalKey);

    bool permanentRemappingExists(const TOriginalKey &originalKey);

    bool isMappedKeyForHyperBinding(THyperKey hyperKey, TOriginalKey originalKey);

    bool isHyperNamesWithoutKeyEmpty();

    void bindHyperNamesWithoutKeys();

private:
    TKeyBindingMap commonHyperBindings;
    THyperKeysMap hyperBindings;
    TKeyBindingMap permanentRemappings;
    THyperNamesMap hyperNamesMap;
    THyperNamesWithoutKey hyperNamesWithoutKey;
};

#endif //BINDINGS_H
