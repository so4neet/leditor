#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include "../logger/logger.h"
#include "../global.h"

GlyphAtlas *create_glyph_atlas(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color) {
    GlyphAtlas *atlas = calloc(1, sizeof(GlyphAtlas));

    TTF_SizeText(font, "M", &atlas->char_w, &atlas->char_h);

    atlas->cols = 10;
    int rows = (ASCII_NUM_CHARS + atlas->cols - 1) / atlas->cols;
    int atlas_w = atlas->cols * atlas->char_w;
    int atlas_h = rows * atlas->char_h;

    SDL_Surface *atlas_surface = SDL_CreateRGBSurfaceWithFormat(0, atlas_w, atlas_h, 32, SDL_PIXELFORMAT_RGBA32);

    // Render each character into a slot
    for (int i=0; i < ASCII_NUM_CHARS; i++) {
        char c = (char)(ASCII_FIRST_CHAR + i);
	SDL_Surface *glyph_surface = TTF_RenderGlyph_Blended(font, c, color);
	if (!glyph_surface) continue;

	int grid_x = (i % atlas->cols) * atlas->char_w;
	int grid_y = (i / atlas->cols) * atlas->char_h;

	SDL_Rect dst_rect = { grid_x, grid_y, glyph_surface->w, glyph_surface->h };

	SDL_SetSurfaceBlendMode(glyph_surface, SDL_BLENDMODE_NONE);
	SDL_BlitSurface(glyph_surface, NULL, atlas_surface, &dst_rect);

	SDL_FreeSurface(glyph_surface);
    }

    atlas->texture = SDL_CreateTextureFromSurface(renderer, atlas_surface);
    SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(atlas_surface);

    return atlas;
}

void destroy_glyph_atlas(GlyphAtlas *atlas) {
    if (!atlas) return;
    if (atlas->texture) SDL_DestroyTexture(atlas->texture);
    free(atlas);
}

void render_char(SDL_Renderer *renderer, GlyphAtlas *atlas, char c, int x, int y) {
    if (c < ASCII_FIRST_CHAR || c > ASCII_LAST_CHAR) c = '?';

    int index = c - ASCII_FIRST_CHAR;

    SDL_Rect src = {
        .x = (index % atlas->cols) * atlas->char_w,
        .y = (index / atlas->cols) * atlas->char_h,
        .w = atlas->char_w,
        .h = atlas->char_h
    };

    SDL_Rect dst = {
        .x = x,
        .y = y,
        .w = atlas->char_w,
        .h = atlas->char_h
    };

    SDL_RenderCopy(renderer, atlas->texture, &src, &dst);
}

void render_line_atlas(SDL_Renderer *renderer, GlyphAtlas *atlas, const char *text, size_t len, int x, int y) {
    for (size_t i = 0; i < len; i++) {
        render_char(renderer, atlas, text[i], x + (int)(i * atlas->char_w), y);
    }
}

void render_buffer(SDL_Renderer *renderer, GlyphAtlas *atlas, InputBuffer *buffer, SDL_Color textColor) {
    int margin_x = 20;
    int margin_y = 20;  // Make these configurable in the future

    int char_w = atlas->char_w;
    int char_h = atlas->char_h;
    int line_height = char_h + 2;  // Make this configurable in the future

    SDL_SetTextureColorMod(atlas->texture, textColor.r, textColor.g, textColor.b);
    SDL_SetTextureAlphaMod(atlas->texture, textColor.a);

    for (size_t i=0; i < buffer->line_count; i++) {
        Line *line = &buffer->lines[i];
        if (line->length == 0) continue;

        int line_y = margin_y + (int)(i * line_height);

        for (size_t col = 0; col < line->length; col++) {
            char c = line->data[col];
            int char_x = margin_x + (int)(col * char_w);

            render_char(renderer, atlas, c, char_x, line_y);
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
