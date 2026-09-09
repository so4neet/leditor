#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "logger/logger.h"
#include "config/config.h"
#include "surface/surface.h"
#include "editor/render.h"

int main(int argc, char **argv) {
  LED_Window *window = malloc(sizeof(LED_Window));
  if (window == NULL) {
    l_fatal("Failed to alloc window memory.");
    return 1;
  } else {
      load_config(window);
      LED_Init_Window(window);
  }

  // Test stuff
  SDL_Color white = {255, 255, 255, 255};
  SDL_Rect text_rect = { .x = 20, .y = 20};

  SDL_Texture *text_texture = create_text_texture(window->renderer, window->font, "Testing text rendering in SDL2!", white, &text_rect);

  while (!window->shouldClose) {
    SDL_Event event = {0};
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
              window->shouldClose = 1;
              break;
        }
    }
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);
    if (text_texture) {
        SDL_RenderCopy(window->renderer, text_texture, NULL, &text_rect);
    }

    SDL_RenderPresent(window->renderer);
  }
  if (text_texture) SDL_DestroyTexture(text_texture);
  TTF_Quit();
  SDL_Quit();
  free(window);
  return 0;
}
