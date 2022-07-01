#include <cstdio>
#include <iostream>
#include <deque>

#include "keys.h"
#include "Config.h"
#include "emit.h"
#include "mapper.h"

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
                emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
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
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    if (!hyperEmitted) {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), 1);
                    }

                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), 0);
                }
            } else {
                if (!isModifier(code) && isDown(value)) {
                    if (!hyperEmitted) {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                        hyperEmitted = true;
                    }
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                } else {
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
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
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, emitQueue.front()), 1);
                    }
                    emitQueue.push_back(code);
                    emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                } else {
                    for (auto& queuedItem : emitQueue) {
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, queuedItem), 1);
                    }
                    emitQueue.clear();
                    emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    if (!hyperEmitted) {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                    }
                    for (auto& queuedItem : emitQueue) {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(queuedItem), 1);
                    }
                    emitQueue.clear();
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 0);
                }
            } else {
                state = map;
                emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
            }
            break;
        }
        case map: // 3
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (isDown(value)) {
                    emitQueue.push_back(code);
                    emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                } else {
                    emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                currentHyperKey = code;
                if (!isDown(value)) {
                    state = idle;
                    for (auto& queuedItem : emitQueue) {
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, queuedItem), 0);
                    }
                    emitQueue.clear();
                }
            } else {
                emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
            }
            break;
        }
    }
    printf("processKey(out): state=%i\n", state);
}
