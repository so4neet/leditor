#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "../global.h"

GlyphAtlas *create_glyph_atlas(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color) {
    GlyphAtlas *atlas = calloc(1, sizeof(GlyphAtlas));
    if (!atlas) return NULL;

    // SDL3_ttf: TTF_SizeText() -> TTF_GetStringSize(), takes a length param and returns bool.
    TTF_GetStringSize(font, "M", 0, &atlas->char_w, &atlas->char_h);

    atlas->cols = 10;
    int rows = (ASCII_NUM_CHARS + atlas->cols - 1) / atlas->cols;
    int atlas_w = atlas->cols * atlas->char_w;
    int atlas_h = rows * atlas->char_h;

    // SDL3: SDL_CreateRGBSurfaceWithFormat() -> SDL_CreateSurface(w, h, format), no flags/depth params.
    SDL_Surface *atlas_surface = SDL_CreateSurface(atlas_w, atlas_h, SDL_PIXELFORMAT_RGBA32);

    // Render each character into a slot
    for (int i=0; i < ASCII_NUM_CHARS; i++) {
        char c = (char)(ASCII_FIRST_CHAR + i);
	SDL_Surface *glyph_surface = TTF_RenderGlyph_Blended(font, (Uint32)c, color);
	if (!glyph_surface) continue;

	int grid_x = (i % atlas->cols) * atlas->char_w;
	int grid_y = (i / atlas->cols) * atlas->char_h;

	SDL_Rect dst_rect = { grid_x, grid_y, glyph_surface->w, glyph_surface->h };

	SDL_SetSurfaceBlendMode(glyph_surface, SDL_BLENDMODE_NONE);
	SDL_BlitSurface(glyph_surface, NULL, atlas_surface, &dst_rect);

	SDL_DestroySurface(glyph_surface); // SDL3: SDL_FreeSurface() -> SDL_DestroySurface()
    }

    atlas->texture = SDL_CreateTextureFromSurface(renderer, atlas_surface);
    SDL_SetTextureBlendMode(atlas->texture, SDL_BLENDMODE_BLEND);
    SDL_DestroySurface(atlas_surface);

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

    // SDL3: renderer draw calls take SDL_FRect (floating point), not SDL_Rect.
    SDL_FRect src = {
        .x = (float)((index % atlas->cols) * atlas->char_w),
        .y = (float)((index / atlas->cols) * atlas->char_h),
        .w = (float)atlas->char_w,
        .h = (float)atlas->char_h
    };

    SDL_FRect dst = {
        .x = (float)x,
        .y = (float)y,
        .w = (float)atlas->char_w,
        .h = (float)atlas->char_h
    };

    SDL_RenderTexture(renderer, atlas->texture, &src, &dst); // SDL3: SDL_RenderCopy() -> SDL_RenderTexture()
}

void render_line_atlas(SDL_Renderer *renderer, GlyphAtlas *atlas, const char *text, size_t len, int x, int y) {
    for (size_t i = 0; i < len; i++) {
        render_char(renderer, atlas, text[i], x + (int)(i * atlas->char_w), y);
    }
}

void render_buffer(SDL_Renderer *renderer, GlyphAtlas *atlas, InputBuffer *buffer, int win_height) {
    int bar_height = atlas->char_h + 6;
        int visible_height = win_height - bar_height;
        size_t visible_rows = visible_height / atlas->char_h;

        for (size_t r = 0; r < visible_rows; r++) {
            size_t line_idx = buffer->scroll_row + r;
            if (line_idx >= buffer->line_count) break;

            Line *line = &buffer->lines[line_idx];

            if (line->length > buffer->scroll_col) {
                const char *visible_text = &line->data[buffer->scroll_col];
                size_t visible_len = line->length - buffer->scroll_col;

                int screen_x = 0;
                int screen_y = (int)(r * atlas->char_h);

                render_line_atlas(renderer, atlas, visible_text, visible_len, screen_x, screen_y);
            }
        }

        if (buffer->cursor_row >= buffer->scroll_row &&
            buffer->cursor_row < buffer->scroll_row + visible_rows &&
            buffer->cursor_col >= buffer->scroll_col)
        {
            int cursor_screen_x = (int)((buffer->cursor_col - buffer->scroll_col) * atlas->char_w);
            int cursor_screen_y = (int)((buffer->cursor_row - buffer->scroll_row) * atlas->char_h);

            SDL_FRect cursor_rect = {
                .x = (float)cursor_screen_x,
                .y = (float)cursor_screen_y,
                .w = (float)atlas->char_w,
                .h = (float)atlas->char_h
            };

            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
            SDL_RenderFillRect(renderer, &cursor_rect);
        }
    }

void render_stat_bar(SDL_Renderer *renderer, GlyphAtlas *atlas, InputBuffer *buffer, InputDispatcher *dispatcher, int win_width, int win_height) {
    int bar_height = atlas->char_h + 6;
    SDL_FRect bar_rect = {0.0f, (float)(win_height - bar_height), (float)win_width, (float)bar_height};

    SDL_SetRenderDrawColor(renderer, 35, 35, 35, 255);
    SDL_RenderFillRect(renderer, &bar_rect);

    char status_text[512] = {0};

    if (dispatcher->active_prompt != PROMPT_NONE) {
        const char *prefix = "::";
        if (dispatcher->active_prompt == PROMPT_SAVE_BUFFER) { prefix = "Save Buffer: "; }
        if (dispatcher->active_prompt == PROMPT_LOAD_BUFFER) { prefix = "Load File: "; }
        snprintf(status_text, sizeof(status_text), "%s%s_", prefix, dispatcher->prompt_buffer);
    } else if (dispatcher->pending_leader.key != 0) {
        snprintf(status_text, sizeof(status_text), "Mod->%c-", (char)dispatcher->pending_leader.key);
    } else {
        snprintf(status_text, sizeof(status_text), "| LED | Row: %zu | Col: %zu | Lines: %zu |", buffer->cursor_row + 1, buffer->cursor_col + 1, buffer->line_count);
    }
    render_line_atlas(renderer, atlas, status_text, strlen(status_text), 0, win_height - bar_height + 3);
}
