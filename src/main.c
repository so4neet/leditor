#include <SDL2/SDL.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "logger/logger.h"
#include "config/config.h"
#include "surface/surface.h"
#include "editor/render.h"
#include "editor/buffer.h"

int main(int argc, char **argv) {
    LED_Window *window = malloc(sizeof(LED_Window));
    if (window == NULL) {
        l_fatal("Failed to alloc window memory.");
        return 1;
    } else {
        load_config(window);
        LED_Init_Window(window);
    }
    SDL_Color text_color = {220, 220, 220, 255};
    window->atlas = create_glyph_atlas(window->renderer, window->font, text_color);
    InputBuffer *buffer = buffer_create();
    if (argc > 1) {
        const char *filename = argv[1];
        if (!buffer_load_file(buffer, filename)) {
            l_error("Failed to open %s, opening empty buffer.", filename);
        }
    }
    SDL_StartTextInput();

    while (!window->shouldClose) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    window->shouldClose = 1;
                    break;

                case SDL_TEXTINPUT:
                    for (int i = 0; event.text.text[i] != '\0'; i++) {
                        buffer_insert_line(buffer, event.text.text[i]);
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_BACKSPACE: buffer_remove_line(buffer); break;
                        case SDLK_RETURN:    buffer_split_line(buffer); break;
                        case SDLK_LEFT:      buffer_move_cursor(buffer, 0, -1); break;
                        case SDLK_RIGHT:     buffer_move_cursor(buffer, 0,  1); break;
                        case SDLK_UP:        buffer_move_cursor(buffer, -1, 0); break;
                        case SDLK_DOWN:      buffer_move_cursor(buffer,  1, 0); break;
                }
                break;
            }
        }
        SDL_SetRenderDrawColor(window->renderer, 20, 20, 20, 255);
        SDL_RenderClear(window->renderer);

        render_buffer(window->renderer, window->atlas, buffer, text_color);
        SDL_RenderPresent(window->renderer);
    }
    SDL_StopTextInput();
    TTF_Quit();
    SDL_Quit();
    free(window);
    return 0;
}
