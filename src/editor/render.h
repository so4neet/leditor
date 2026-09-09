#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *out_rect);
