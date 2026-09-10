#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "global.h"
#include "logger/logger.h"
#include "config/config.h"
#include "surface/surface.h"
#include "editor/render.h"
#include "editor/buffer.h"
#include "editor/input.h"

int main(int argc, char **argv) {
    LED_Window *window = calloc(1, sizeof(LED_Window));
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

    InputDispatcher dispatcher = {0};

    SDL_StartTextInput(window->surface);

    while (!window->shouldClose) {
        // Make sure the window knows what size it is. Probably inefficient but oh well.
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                    SDL_GetCurrentRenderOutputSize(window->renderer, &window->width, &window->height);
                    break;

                case SDL_EVENT_QUIT:
                    window->shouldClose = 1;
                    break;

                case SDL_EVENT_TEXT_INPUT:
                    // stat-bar prompt
                    if (dispatcher.active_prompt != PROMPT_NONE) {
                        for (int i = 0; event.text.text[i] != '\0'; i++) {
                            char c = event.text.text[i];
                            if (c >= 32 && c <= 126 && dispatcher.prompt_len < sizeof(dispatcher.prompt_buffer) - 1) {
                                dispatcher.prompt_buffer[dispatcher.prompt_len++] = c;
                                dispatcher.prompt_buffer[dispatcher.prompt_len] = '\0';
                            }
                        }
                    }
                    // text editing
                    else if (dispatcher.pending_leader.key == 0) {
                        for (int i = 0; event.text.text[i] != '\0'; i++) {
                            unsigned char c = (unsigned char)event.text.text[i];
                            if (c >= 32 && c <= 126) {
                                line_insert_single(&buffer->lines[buffer->cursor_row], buffer->cursor_col, (char)c);
                                buffer->cursor_col++;
                            }
                        }
                    }
                    break;

                case SDL_EVENT_KEY_DOWN: {
                    // stat-bar prompt
                    if (dispatcher.active_prompt != PROMPT_NONE) {
                        // SDL3: event.key.keysym.sym -> event.key.key
                        if (event.key.key == SDLK_ESCAPE) {
                            // exit stat-bar
                            dispatcher.active_prompt = PROMPT_NONE;
                            dispatcher.prompt_len = 0;
                            dispatcher.prompt_buffer[0] = '\0';
                        } else if (event.key.key == SDLK_BACKSPACE) {
                            if (dispatcher.prompt_len > 0) {
                                dispatcher.prompt_buffer[--dispatcher.prompt_len] = '\0';
                            }
                        } else if (event.key.key == SDLK_RETURN) {
                            // run command
                            if (dispatcher.prompt_len > 0) {
                                if (dispatcher.active_prompt == PROMPT_SAVE_BUFFER) {
                                    buffer_save_file(buffer, dispatcher.prompt_buffer);
                                } else if (dispatcher.active_prompt == PROMPT_LOAD_BUFFER) {
                                    buffer_load_file(buffer, dispatcher.prompt_buffer);
                                }
                            }
                            dispatcher.active_prompt = PROMPT_NONE;
                            dispatcher.prompt_len = 0;
                            dispatcher.prompt_buffer[0] = '\0';
                        }
                        break;
                    }
                    // text editing
                    if (event.key.key == SDLK_BACKSPACE) {
                        buffer_remove_line(buffer);
                        break;
                    }
                    if (event.key.key == SDLK_RETURN) {
                        buffer_split_line(buffer);
                        break;
                    }

                    EditorAction action = dispatch_key(&dispatcher, &event);
                    switch (action) {
                        case ACTION_MOVE_LEFT:  buffer_move_cursor(buffer, 0, -1); break;
                        case ACTION_MOVE_RIGHT: buffer_move_cursor(buffer, 0,  1); break;
                        case ACTION_MOVE_UP:    buffer_move_cursor(buffer, -1, 0); break;
                        case ACTION_MOVE_DOWN:  buffer_move_cursor(buffer,  1, 0); break;
                        case ACTION_TRIGGER_LOAD: dispatcher.active_prompt = PROMPT_LOAD_BUFFER; break;
                        case ACTION_TRIGGER_SAVE: dispatcher.active_prompt = PROMPT_SAVE_BUFFER; break;
                        case ACTION_CLOSE_WIN: window->shouldClose = 1; break;
                            //case ACTION_NEW_BUFFER: buffer_clear(buffer);
                        case ACTION_NONE:
                        default:
                            break;
                    }
                    break;
                }
            }
        }
        buffer_clamp_scroll(buffer, window->atlas, window->width, window->height);
        SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window->renderer, 20, 20, 20, 204);
        SDL_RenderClear(window->renderer);

        render_buffer(window->renderer, window->atlas, buffer, window->height);
        render_stat_bar(window->renderer, window->atlas, buffer, &dispatcher, window->width, window->height);
        SDL_RenderPresent(window->renderer);
    }
    SDL_StopTextInput(window->surface); // SDL3: also takes the window now
    destroy_buffer(buffer);
    destroy_glyph_atlas(window->atlas);
    TTF_Quit();
    SDL_Quit();
    free(window);
    return 0;
}
