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
 * Processes a key input event. Converts and emits events as necessary.
 */
void processKey(int type, int code, int value) {
    static std::deque<TMappedKey> emitQueue{};

    // The state machine state.
    static States state{ States::idle };

    static THyperKey currentHyperKey{0};

    // Flag if the hyper key has been emitted.
    static bool hyperPressedDownEmitted;

    //printf("processKey(in): code=%i value=%i state=%i\n", code, value, state);
    switch (state) {
        case States::idle:
        {
            if (config.bindings.hyperKeyExists(code) && isDown(value)) {
                //std::cout << "Idle: Hyper key " << code << " pressed.\n";
                state = States::hyper;
                currentHyperKey = code;
                hyperPressedDownEmitted = false;
                emitQueue.clear();
            } else { // If hyper key exists but is being released, or the key is a normal, non-hyper key code.
                emitPermanentRemapping(type, code, value);
            }
            break;
        }
        case States::hyper:
        {
            // Once a hyper key is pressed, we do not want to be able to change to bindings for other hyper keys. Therefore,
            // we would like to possibly emit a mapped key binding for the already pressed hyper binding.
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                // A hyper key is being held down and a new key is pressed or released.
                if (isDown(value)) {
                    state = States::delay;
                    emitQueue.push_back(code);
                } else { // isUp(value)
                    emitPermanentRemapping(type, code, value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                if (currentHyperKey == code) {
                    if (isUp(value)) {
                        // The currentHyperKey is being released without emitting any hyper bindings. We want to emit the
                        // key codes as if the key was a normal key (not a hyper key).

                        if (!hyperPressedDownEmitted) {
                            emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                            hyperPressedDownEmitted = true;
                        }
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_UP);

                        state = States::idle;
                    }
                } else { // It is a different hyper key, but does not have any hyper key binding for the currentHyperKey.
                    // FIXME.
                    state = States::idle;

                    if (!isModifier(code) && isDown(value)) {
                        if (!hyperPressedDownEmitted) {
                            emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                            hyperPressedDownEmitted = true;
                        }
                        emitPermanentRemapping(type, code, value);
                    } else if (!hyperPressedDownEmitted) {
                        emitPermanentRemapping(type, code, EVENT_KEY_DOWN);
                        emitPermanentRemapping(type, code, EVENT_KEY_UP);
                    } else {
                        emitPermanentRemapping(type, code, value);
                    }
                }
            } else { // Not hyper key binding, nor a hyper key.
                // FIXME.
                if (!isModifier(code) && isDown(value)) {
                    if (!hyperPressedDownEmitted) {
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperPressedDownEmitted = true;
                    }
                    emitPermanentRemapping(type, code, value);
                } else {
                    if (!hyperPressedDownEmitted) {
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperPressedDownEmitted = true;
                    }
                    emitPermanentRemapping(type, code, value);
                }
            }
            break;
        }
        case States::delay:
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (emitQueue.front() == code) {
                    if (isDown(value)) {
                        // For the previous hyper binding in State::hyper.
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                        emitQueue.push_back(code);
                    } else {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_UP);
                        emitQueue.pop_front();

                        // The whole mapped hyper binding down-up sequence finished. Hyper key remains pressed, waiting
                        //  to emit another hyper key binding.
                    }
                } else { // Not same key.
                    emitHyperBinding(type, currentHyperKey, emitQueue.front(), EVENT_KEY_DOWN);
                    emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                    emitQueue.push_back(code);
                }

                // At least one hyper binding was emitted. THe hyper key is being used as a hyper key, not just to emit
                //  the hyper key code.
                state = States::map;

            } else if (config.bindings.hyperKeyExists(code)) {
                if (currentHyperKey == code && isUp(value)) {

                    if (!hyperPressedDownEmitted) {
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                    }

                    for (const auto &queuedItem: emitQueue) {
                        emitPermanentRemapping(type, queuedItem, EVENT_KEY_DOWN);
                    }
                    emitQueue.clear();

                    emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_UP);

                    state = States::idle;
                } else {
                    // FIXME.
                    state = States::map;
                    emitPermanentRemapping(type, code, value);
                }
            }
            break;
        }
        case States::map:
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (emitQueue.front() == code) {
                    if (isDown(value)) {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                        emitQueue.push_back(code);
                    } else {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_UP);
                        emitQueue.pop_front();

                        // The whole mapped hyper binding down-up sequence finished. Hyper key remains pressed, waiting
                        //  to emit another hyper key binding.
                    }
                } else { // Not same key.
                    if (isDown(value)) {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                        emitQueue.push_back(code);
                    } else {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_UP);
                        emitQueue.pop_front();
                    }
                }
            } else if (config.bindings.hyperKeyExists(code)) {
                if (code == currentHyperKey && isUp(value)) {
                    // Finish emitting the hyper key mappings whose DOWN key codes were emitted while holding the hyper
                    //  key pressed.
                    for (const auto& queuedItem : emitQueue) {
                        emitHyperBinding(type, currentHyperKey, queuedItem, EVENT_KEY_UP);
                    }
                    emitQueue.clear();

                    state = States::idle;
                } else {
                    emitPermanentRemapping(type, code, value);
                }
            } else {
                emitPermanentRemapping(type, code, value);
            }
            break;
        }
    }
    //printf("processKey(out): state=%i\n", state);
}

void emitPermanentRemapping(int type, int code, int value) {
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

void emitHyperBinding(int type, int hyperKeyCode, int originalKeyCode, int value) {
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
