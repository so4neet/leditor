#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
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

    // 1. Ignore pure modifier key events (e.g. pressing/releasing Ctrl itself)
    if (key == SDLK_LCTRL || key == SDLK_RCTRL ||
        key == SDLK_LSHIFT || key == SDLK_RSHIFT ||
        key == SDLK_LALT || key == SDLK_RALT ||
        key == SDLK_LGUI || key == SDLK_RGUI) {
        return ACTION_NONE;
    }

    uint16_t mod = clean_modifiers(event->key.keysym.mod);
    KeyCombo current_input = { key, mod };

    // 2. Check chord timeout (e.g. 1 second elapsed)
    if (dispatcher->pending_leader.key != 0) {
        if (SDL_GetTicks() - dispatcher->leader_timestamp > 1000) {
            dispatcher->pending_leader = (KeyCombo){0, 0};
        }
    }

    // 3. Process Secondary Key in a Chord
    if (dispatcher->pending_leader.key != 0) {
        KeyCombo leader = dispatcher->pending_leader;

        for (size_t i = 0; i < BINDING_COUNT; i++) {
            const ChordBinding *b = &BIND_TABLE[i];

            // Match full chord: Leader + Trigger
            if (b->leader.key == leader.key &&
                b->leader.mod == leader.mod &&
                b->trigger.key == current_input.key &&
                b->trigger.mod == current_input.mod)
            {
                dispatcher->pending_leader = (KeyCombo){0, 0}; // Clear on success
                return b->action;
            }
        }

        // Unmatched secondary key sequence: reset leader state
        dispatcher->pending_leader = (KeyCombo){0, 0};
        return ACTION_NONE;
    }

    // 4. Check if current key is a Leader for a Chord (e.g. C-x)
    for (size_t i = 0; i < BINDING_COUNT; i++) {
        const ChordBinding *b = &BIND_TABLE[i];
        if (b->leader.key == current_input.key &&
            b->leader.mod == current_input.mod &&
            b->trigger.key != 0)
        {
            dispatcher->pending_leader = current_input;
            dispatcher->leader_timestamp = SDL_GetTicks();
            return ACTION_NONE;
        }
    }

    // 5. Check Single Key / Direct Bindings
    for (size_t i = 0; i < BINDING_COUNT; i++) {
        const ChordBinding *b = &BIND_TABLE[i];
        if (b->leader.key == 0 &&
            b->trigger.key == current_input.key &&
            b->trigger.mod == current_input.mod)
        {
            return b->action;
        }
    }

    return ACTION_NONE;
}
