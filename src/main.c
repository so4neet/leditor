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
    InputBuffer *buffer = buffer_create();
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
                        buffer_insert(buffer, event.text.text[i]);
                    }
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_BACKSPACE:
                            buffer_remove(buffer);
                            break;
                        case SDLK_LEFT:
                            if (buffer->cursor > 0) buffer->cursor--;
                            break;
                        case SDLK_RIGHT:
                            if (buffer->cursor < buffer->length) buffer->cursor++;
                            break;
                        case SDLK_RETURN:
                            buffer_insert(buffer, '\n');
                            break;
                }
            }
        }
        SDL_SetRenderDrawColor(window->renderer, 20, 20, 20, 255);
        SDL_RenderClear(window->renderer);

        SDL_Color text_color = {220, 220, 220, 255};
        render_buffer(window->renderer, window->font, buffer, text_color);
        SDL_RenderPresent(window->renderer);
    }
    SDL_StopTextInput();
    TTF_Quit();
    SDL_Quit();
    if (buffer) free_buffer(buffer);
    free(window);
    return 0;
}
