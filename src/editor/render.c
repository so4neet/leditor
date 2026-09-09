#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include "../logger/logger.h"
#include "../global.h"

void render_buffer(SDL_Renderer *renderer, TTF_Font *font, InputBuffer *buffer, SDL_Color textColor) {
    int margin_x = 20;
    int margin_y = 20;  // Make these configurable in the future

    int char_w = 0, char_h = 0;
    TTF_SizeText(font, "A", &char_w, &char_h);
    int line_height = char_h + 2;  // Make this configurable in the future

    for (size_t i=0; i < buffer->line_count; i++) {
        Line *line = &buffer->lines[i];
        if (line->length == 0) continue;

        SDL_Surface *surface = TTF_RenderText_Blended(font, line->data, textColor);
        if (surface) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect dst = {
                .x = margin_x,
                .y = margin_y + (int)(i * line_height),
                .w = surface->w,
                .h = surface->h
            };
            SDL_RenderCopy(renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surface);
        }
    }

    int cursor_x = margin_x + (int)(buffer->cursor_col * char_w);
    int cursor_y = margin_y + (int)(buffer->cursor_row * line_height);

    SDL_Rect cursor_rect = {
        .x = cursor_x,
        .y = cursor_y,
        .w = char_w,
        .h = char_h
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &cursor_rect);
}
