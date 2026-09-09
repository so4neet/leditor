#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../global.h"

void render_buffer(SDL_Renderer *renderer, GlyphAtlas *atlas, InputBuffer *buffer, SDL_Color textColor);
GlyphAtlas *create_glyph_atlas(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color);
void destroy_glyph_atlas(GlyphAtlas *atlas);
void render_char(SDL_Renderer *renderer, GlyphAtlas *atlas, char c, int x, int y);
void render_line_atlas(SDL_Renderer *renderer, GlyphAtlas *atlas, const char *text, size_t len, int x, int y);
