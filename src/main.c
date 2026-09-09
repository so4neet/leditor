#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "global.h"
#include "logger/logger.h"
#include "config/config.h"
#include "surface/surface.h"

int main(int argc, char **argv) {
  LED_Window *window = malloc(sizeof(LED_Window));
  if (window == NULL) {
    l_fatal("Failed to alloc window memory.");
    return 1;
  } else {
      load_config(window);
      LED_Init_Window(window);
  }

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
    SDL_RenderPresent(window->renderer);
  }

  SDL_Quit();
  free(window);
  return 0;
}
