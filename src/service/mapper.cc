#include <cstdio>
#include <iostream>
#include <deque>
#include <algorithm>

#include "keys.h"
#include "Config.h"
#include "emit.h"
#include "mapper.h"
#include "KeyCodes.h"


/**
 * * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(Config &config, int type, int code, int value) {
    static std::deque<TMappedKey> emitQueue{};
    // The state machine state.
    static enum states state = idle;

    static THyperKey currentHyperKey{0};

    // Flag if the hyper key has been emitted.
    static bool hyperEmitted;

    printf("processKey(in): code=%i value=%i state=%i\n", code, value, state);
    switch (state) {
        case idle: // 0
        {
            if (config.bindings.hyperKeyExists(code) && isDown(value)) {
                std::cout << "Idle: Hyper key " << code << " pressed.\n";
                state = hyper;
                currentHyperKey = code;
                hyperEmitted = false;
                emitQueue.clear();
            } else {
                emitPermanentRemapping(config, type, code, value);
            }
            break;
        }
        case hyper: // 1
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (isDown(value)) {
                    state = delay;
                    emitQueue.push_back(code);
                } else {
                    emitPermanentRemapping(config, type, code, value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    if (!hyperEmitted) {
                        emitPermanentRemapping(config, type, code, EVENT_KEY_DOWN);
                    }

                    emitPermanentRemapping(config, type, code, EVENT_KEY_UP);
                }
            } else {
                if (!isModifier(code) && isDown(value)) {
                    if (!hyperEmitted) {
                        emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperEmitted = true;
                    }
                    emitPermanentRemapping(config, type, code, value);
                } else {
                    emitPermanentRemapping(config, type, code, value);
                }
            }
            break;
        }
        case delay: // 2
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                state = map;
                if (isDown(value)) {
                    if (!emitQueue.empty()) {
                        emitHyperBinding(config, type, currentHyperKey, emitQueue.front(), EVENT_KEY_DOWN);
                    }
                    emitQueue.push_back(code);
                    emitHyperBinding(config, type, currentHyperKey, code, value);
                } else {
                    for (auto& queuedItem : emitQueue) {
                        emitHyperBinding(config, type, currentHyperKey, queuedItem, EVENT_KEY_DOWN);
                    }
                    emitQueue.clear();
                    emitHyperBinding(config, type, currentHyperKey, code, value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    if (!hyperEmitted) {
                        emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_DOWN);
                    }
                    for (auto& queuedItem : emitQueue) {
                        emitPermanentRemapping(config, type, queuedItem, EVENT_KEY_DOWN);
                    }
                    emitQueue.clear();
                    emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_UP);
                }
            } else {
                state = map;
                emitPermanentRemapping(config, type, code, value);
            }
            break;
        }
        case map: // 3
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (isDown(value)) {
                    emitQueue.push_back(code);
                    emitHyperBinding(config, type, currentHyperKey, code, value);
                } else {
                    emitHyperBinding(config, type, currentHyperKey, code, value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    for (auto& queuedItem : emitQueue) {
                        emitHyperBinding(config, type, currentHyperKey, queuedItem, 0);
                    }
                    emitQueue.clear();
                }
            } else {
                emitPermanentRemapping(config, type, code, value);
            }
            break;
        }
    }
    printf("processKey(out): state=%i\n", state);
}

void emitPermanentRemapping(Config &config, int type, int code, int value) {
    if (config.bindings.isMappedKeyForPermanentRemapping(code)) {
        auto& sequenceList = config.bindings.getMappedKeyForPermanentRemapping(code);
        for (auto& combinationList: sequenceList) {
            if (value == EVENT_KEY_UP) {
                if (sequenceList.size() <= 1 && combinationList.size() <= 1) {
                    std::for_each(combinationList.rbegin(), combinationList.rend(), [&](TMappedKey& mappedCode) {
                        emit(type, mappedCode, value);
                    });
                }
            } else {
                std::for_each(combinationList.begin(), combinationList.end(), [&](TMappedKey& mappedCode) {
                    emit(type, mappedCode, value);
                    if (sequenceList.size() > 1 && combinationList.size() <= 1) {
                        emit(type, mappedCode, EVENT_KEY_UP);
                    }
                });

                if (combinationList.size() > 1) {
                    std::for_each(combinationList.rbegin(), combinationList.rend(), [&](TMappedKey& mappedCode) {
                        emit(type, mappedCode, EVENT_KEY_UP);
                    });
                }

            }
        }
    } else {
        emit(type, code, value);
    }
}

void emitHyperBinding(Config &config, int type, int hyperKeyCode, int originalKeyCode, int value) {
    if (config.bindings.isMappedKeyForHyperBinding(hyperKeyCode, originalKeyCode)) {
        auto& sequenceList = config.bindings.getMappedKeyForHyperBinding(hyperKeyCode, originalKeyCode);
        for (auto& combinationList: sequenceList) {
            if (value == EVENT_KEY_UP) {
                if (sequenceList.size() <= 1 && combinationList.size() <= 1) {
                    std::for_each(combinationList.rbegin(), combinationList.rend(), [&](TMappedKey& mappedCode) {
                        emit(type, mappedCode, value);
                    });
                }
            } else {
                std::for_each(combinationList.begin(), combinationList.end(), [&](TMappedKey& mappedCode) {
                    emit(type, mappedCode, value);
                    if (sequenceList.size() > 1 && combinationList.size() <= 1) {
                        emit(type, mappedCode, EVENT_KEY_UP);
                    }
                });

                if (combinationList.size() > 1) {
                    std::for_each(combinationList.rbegin(), combinationList.rend(), [&](TMappedKey& mappedCode) {
                        emit(type, mappedCode, EVENT_KEY_UP);
                    });
                }

            }
        }
    } else {
        emit(type, originalKeyCode, value);
    }
}
