#include <cstdio>
#include <linux/input.h>
#include <linux/uinput.h>
#include <iostream>

#include "queue.h"
#include "keys.h"
#include "config.h"
#include "emit.h"
#include "mapper.h"

// The state machine state
enum states state = idle;

// Flag if the hyper key has been emitted
static int hyperEmitted;

static THyperKey currentHyperKey{ 0 };

/**
 * * Processes a key input event. Converts and emits events as necessary.
 * */
void processKey(Bindings& bindings, int type, int code, int value)
{
    printf("processKey(in): code=%i value=%i state=%i\n", code, value, state);
    switch (state)
    {
        case idle: // 0
            {
                if (bindings.hyperKeyExists(code) && isDown(value))
                {
                    std::cout << "Idle: Hyper key " << code << " pressed.\n";
                    state = hyper;
                    currentHyperKey = code;
                    hyperEmitted = 0;
                    clearQueue();
                }
                else
                {
                    emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
        case hyper: // 1
            {
                if (bindings.hyperKeyExists(code)) {
                    currentHyperKey = code;
                    if (!isDown(value)) {
                        state = idle;
                        if (!hyperEmitted) {
                            emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), 1);
                        }

                        emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), 0);
                    }
                }
                else if (bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    if (isDown(value))
                    {
                        state = delay;
                        enqueue(code);
                    }
                    else
                    {
                        emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                }
                else
                {
                    if (!isModifier(code) && isDown(value))
                    {
                        if (!hyperEmitted)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                            hyperEmitted = 1;
                        }
                        emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                    else
                    {
                        emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                    }
                }
                break;
            }
        case delay: // 2
            {
                if (bindings.hyperKeyExists(code))
                {
                    currentHyperKey = code;
                    if (!isDown(value))
                    {
                        state = idle;
                        if (!hyperEmitted)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 1);
                        }
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(dequeue()), 1);
                        }
                        emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(currentHyperKey), 0);
                    }
                }
                else if (bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    state = map;
                    if (isDown(value))
                    {
                        if (lengthOfQueue() != 0)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, peek()), 1);
                        }
                        enqueue(code);
                        emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                    else
                    {
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, dequeue()), 1);
                        }
                        emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                }
                else
                {
                    state = map;
                    emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
        case map: // 3
            {
                if (bindings.hyperKeyExists(code))
                {
                    currentHyperKey = code;
                    if (!isDown(value))
                    {
                        state = idle;
                        int length = lengthOfQueue();
                        for (int i = 0; i < length; i++)
                        {
                            emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, dequeue()), 0);
                        }
                    }
                }
                else if (bindings.isMappedKeyForHyperBinding(currentHyperKey, code))
                {
                    if (isDown(value))
                    {
                        enqueue(code);
                        emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                    else
                    {
                        emit(EV_KEY, bindings.getMappedKeyForHyperBinding(currentHyperKey, code), value);
                    }
                }
                else
                {
                    emit(EV_KEY, bindings.getMappedKeyForPermanentRemapping(code), value);
                }
                break;
            }
    }
    printf("processKey(out): state=%i\n", state);
}
