#include <SDL2/SDL.h>
#include "../global.h"

uint16_t clean_modifiers(uint16_t mod) {
    uint16_t cleaned = 0;
    if (mod & KMOD_CTRL)    cleaned |= KMOD_CTRL;
    if (mod & KMOD_ALT)     cleaned |= KMOD_ALT;
    if (mod & KMOD_SHIFT)   cleaned |= KMOD_SHIFT;
    if (mod & KMOD_GUI)     cleaned |= KMOD_GUI;
    return cleaned;
}

EditorAction dispatch_key(InputDispatcher *dispatcher, SDL_Event *event) {
    SDL_Keycode key = event->key.keysym.sym;
    uint16_t mod = clean_modifiers(event->key.keysym.mod);
    KeyCombo current_input = { key, mod };

    // timeout
    if (dispatcher->pending_leader.key != 0) {
        if (SDL_GetTicks() - dispatcher->leader_timestamp > 1000) {
            dispatcher->pending_leader = (KeyCombo){0, 0};
        }
    }

    // secondary key
    if (dispatcher->pending_leader.key != 0) {
        KeyCombo leader = dispatcher->pending_leader;
        dispatcher->pending_leader = (KeyCombo){0, 0};  // reset state

        for (size_t i = 0; i < BINDING_COUNT; i++) {
            const ChordBinding *b = &BIND_TABLE[i];
            if (b->leader.key == leader.key && b->leader.mod == leader.mod && b->trigger.key == current_input.key && b->trigger.mod == current_input.mod) {
                return b->action;
            }
        }
        return ACTION_NONE;
    }

    for (size_t i = 0; i < BINDING_COUNT; i++) {
        const ChordBinding *b = &BIND_TABLE[i];
        if (b->leader.key == current_input.key && b->leader.mod == current_input.mod && b->trigger.key != 0) {
            dispatcher->pending_leader = current_input;
            dispatcher->leader_timestamp = SDL_GetTicks();
            return ACTION_NONE;
        }
    }

    for (size_t i = 0; i < BINDING_COUNT; i++) {
        const ChordBinding *b = &BIND_TABLE[i];
        if (b->leader.key == 0 && b->trigger.key == current_input.key && b->trigger.mod == current_input.mod) {
            return b->action;
        }
        if (b->leader.key == current_input.key && b->leader.mod == current_input.mod && b->trigger.key == 0) {
            return b->action;
        }
    }
    return ACTION_NONE;
}
