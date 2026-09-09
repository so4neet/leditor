#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../global.h"

void render_buffer(SDL_Renderer *renderer, TTF_Font *font, InputBuffer *buffer, SDL_Color textColor);
