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
    static bool hyperEmitted;

    //printf("processKey(in): code=%i value=%i state=%i\n", code, value, state);
    switch (state) {
        case States::idle:
        {
            if (config.bindings.hyperKeyExists(code) && isDown(value)) {
                //std::cout << "Idle: Hyper key " << code << " pressed.\n";
                state = States::hyper;
                currentHyperKey = code;
                hyperEmitted = false;
                emitQueue.clear();
            } else {
                emitPermanentRemapping(type, code, value);
            }
            break;
        }
        case States::hyper: // 1
        {
            // Once a hyper key is pressed, we do not want to be able to change to bindings for other hyper keys. Therefore,
            // we would like to possibly emit a mapped key binding for the already pressed hyper binding.
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                // A hyper key is being held down and a new key is pressed.
                if (isDown(value)) {
                    state = States::delay;
                    emitQueue.push_back(code);
                } else {
                    emitPermanentRemapping(type, code, value);
                }
            } else if (config.bindings.hyperKeyExists(code)) {
            //    // FIXME: This whole branch is possibly wrong. When I am already holding a hyper key down, I do NOT
            //    //  want to change to a different hyper key, right?

            //    if (!isDown(value)) {
                    state = idle;

                if (!isModifier(code) && isDown(value)) {
                    if (!hyperEmitted) {
                        emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperEmitted = true;
                    }
                    emitPermanentRemapping(config, type, code, value);
                } else if (!hyperEmitted) {
                    emitPermanentRemapping(config, type, code, EVENT_KEY_DOWN);
                    emitPermanentRemapping(config, type, code, EVENT_KEY_UP);
                } else {
                    emitPermanentRemapping(config, type, code, value);
                }
           //    }

            } else {
                if (!isModifier(code) && isDown(value)) {
                    if (!hyperEmitted) {
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperEmitted = true;
                    }
                    emitPermanentRemapping(type, code, value);
                } else {
                    if (!hyperEmitted) {
                        emitPermanentRemapping(type, currentHyperKey, EVENT_KEY_DOWN);
                        hyperEmitted = true;
                    }
                    emitPermanentRemapping(type, code, value);
                }
            }
            break;
        }
        case States::delay: // 2
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                state = States::map;
                //if (isDown(value)) {
                //    if (!emitQueue.empty()) {
                //        emitHyperBinding(type, currentHyperKey, emitQueue.front(), EVENT_KEY_DOWN);
                //        //emitQueue.pop_front();
                //    }
                //    emitQueue.push_back(code);
                //    emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                //} else {
                    //for (const auto& queuedItem : emitQueue) {
                        bool differentKey{ emitQueue.front() != code };
                        if (differentKey) {
                            emitHyperBinding(type, currentHyperKey, emitQueue.front(), EVENT_KEY_DOWN);
                            // FIXME: Probably not desired to emit DOWN UP for holding a key pressed down, right?
                            //  Wait up, that is exactly what a normal
                            //emitHyperBinding(type, currentHyperKey, emitQueue.front(), EVENT_KEY_UP);
                        }
                    //}

                    emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                    if (!differentKey) {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_UP);
                        emitQueue.clear();
                    } else {
                        emitQueue.push_back(code);
                    }
                //}
            } else if (config.bindings.hyperKeyExists(code)) {
                if (!isDown(value)) {
                    state = idle;
                    if (!hyperEmitted) {
                        emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_DOWN);
                        //emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_UP);
                    }
                    for (auto& queuedItem : emitQueue) {
                        emitPermanentRemapping(config, type, queuedItem, EVENT_KEY_DOWN);
                        emitPermanentRemapping(config, type, currentHyperKey, EVENT_KEY_UP);
                    }
                    emitQueue.clear();
                }
            } else {
                state = States::map;
                emitPermanentRemapping(type, code, value);
            }
            break;
        }
        case States::map: // 3
        {
            if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code)) {
                if (isDown(value)) {
                    emitQueue.push_back(code);
                    emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_DOWN);
                } else {
                    if (emitQueue.front() == code) {
                        emitHyperBinding(type, currentHyperKey, code, EVENT_KEY_UP);
                        emitQueue.clear();
                    } else {

                    }

                }
            } else if (config.bindings.hyperKeyExists(code)) {
                if (code == currentHyperKey && !isDown(value)) {
                    state = States::idle;

                    // Finish emitting the hyper key mappings whose DOWN key codes were emitted while holding the hyper
                    // key pressed.
                    for (const auto& queuedItem : emitQueue) {
                        emitHyperBinding(type, currentHyperKey, queuedItem, EVENT_KEY_UP);
                    }
                    emitQueue.clear();
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
