#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
#include "../global.h"

uint16_t clean_modifiers(uint16_t mod) {
    uint16_t cleaned = 0;
    if (mod & SDL_KMOD_CTRL)    cleaned |= SDL_KMOD_CTRL;
    if (mod & SDL_KMOD_ALT)     cleaned |= SDL_KMOD_ALT;
    if (mod & SDL_KMOD_SHIFT)   cleaned |= SDL_KMOD_SHIFT;
    if (mod & SDL_KMOD_GUI)     cleaned |= SDL_KMOD_GUI;
    return cleaned;
}

EditorAction dispatch_key(InputDispatcher *dispatcher, SDL_Event *event) {
    // event->key.keysym.sym -> event->key.key,
    // event->key.keysym.mod -> event->key.mod.
    SDL_Keycode key = event->key.key;

    if (key == SDLK_LCTRL || key == SDLK_RCTRL ||
        key == SDLK_LSHIFT || key == SDLK_RSHIFT ||
        key == SDLK_LALT || key == SDLK_RALT ||
        key == SDLK_LGUI || key == SDLK_RGUI) {
        return ACTION_NONE;
    }

    uint16_t mod = clean_modifiers(event->key.mod);
    KeyCombo current_input = { key, mod };

    if (dispatcher->pending_leader.key != 0) {
        if (SDL_GetTicks() - dispatcher->leader_timestamp > 1000) {
            dispatcher->pending_leader = (KeyCombo){0, 0};
        }
    }

    if (dispatcher->pending_leader.key != 0) {
        KeyCombo leader = dispatcher->pending_leader;

        for (size_t i = 0; i < BINDING_COUNT; i++) {
            const ChordBinding *b = &BIND_TABLE[i];

            if (b->leader.key == leader.key &&
                b->leader.mod == leader.mod &&
                b->trigger.key == current_input.key &&
                b->trigger.mod == current_input.mod)
            {
                dispatcher->pending_leader = (KeyCombo){0, 0}; // Clear on success
                return b->action;
            }
        }

        dispatcher->pending_leader = (KeyCombo){0, 0};
        return ACTION_NONE;
    }

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
