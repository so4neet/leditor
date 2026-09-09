#pragma once
#include <SDL2/SDL.h>
#include "../global.h"

uint16_t clean_modifiers(uint16_t mod);
EditorAction dispatch_key(InputDispatcher *dispatcher, SDL_Event *event);
