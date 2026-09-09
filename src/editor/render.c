#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../logger/logger.h"

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, SDL_Rect *out_rect) {
    if (!font || !text) return NULL;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) {
        l_error("TTF_RenderText_Blended failure: %s", TTF_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        l_error("SDL_CreateTextureFromSurface failure: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    if (out_rect) {
        out_rect->w = surface->w;
        out_rect->h = surface->h;
    }
    SDL_FreeSurface(surface);
    return texture;
}
