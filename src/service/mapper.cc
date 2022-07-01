#include <cstdio>
#include <iostream>

#include "queue.h"
#include "keys.h"
#include "Config.h"
#include "emit.h"
#include "mapper.h"

/**
 * * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(Config& config, int type, int code, int value)
{
    // The state machine state
    static enum states state = idle;

    static THyperKey currentHyperKey{ 0 };

    // Flag if the hyper key has been emitted
    static int hyperEmitted;

    printf("processKey(in): code=%i value=%i state=%i\n", code, value, state);
    switch (state)
    {
        case idle: // 0
            {
                if (config.bindings.hyperKeyExists(code) && isDown(value))
                {
                    std::cout << "Idle: Hyper key " << code << " pressed.\n";
                    state = hyper;
                    currentHyperKey = code;
                    hyperEmitted = 0;
                    clearQueue();
                }
                else
                {
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
        case hyper: // 1
            {
                if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    if (isDown(value))
                    {
                        state = delay;
                        enqueue(code);
                    }
                    else
                    {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                }
                else if (config.bindings.hyperKeyExists(code)) {
                    currentHyperKey = code;
                    if (!isDown(value)) {
                        state = idle;
                        if (!hyperEmitted) {
                            emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), 1);
                        }

                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), 0);
                    }
                }
                else
                {
                    if (!isModifier(code) && isDown(value))
                    {
                        if (!hyperEmitted)
                        {
                            emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                            hyperEmitted = 1;
                        }
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                    else
                    {
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                }
                break;
            }
        case delay: // 2
            {
                if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    state = map;
                    if (isDown(value))
                    {
                        if (lengthOfQueue() != 0)
                        {
                            emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, peek()), 1);
                        }
                        enqueue(code);
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                    else
                    {
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, dequeue()), 1);
                        }
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                }
                else if (config.bindings.hyperKeyExists(code))
                {
                    currentHyperKey = code;
                    if (!isDown(value))
                    {
                        state = idle;
                        if (!hyperEmitted)
                        {
                            emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                        }
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(type, config.bindings.getMappedKeyForPermanentRemapping(dequeue()), 1);
                        }
                        emit(type, config.bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 0);
                    }
                }
                else
                {
                    state = map;
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
        case map: // 3
            {
                if (config.bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    if (isDown(value))
                    {
                        enqueue(code);
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                    else
                    {
                        emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                }
                else if (config.bindings.hyperKeyExists(code))
                {
                    currentHyperKey = code;
                    if (!isDown(value))
                    {
                        state = idle;
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(type, config.bindings.getMappedKeyForHyperBinding(currentHyperKey, dequeue()), 0);
                        }
                    }
                }
                else
                {
                    emit(type, config.bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
    }
    printf("processKey(out): state=%i\n", state);
}
