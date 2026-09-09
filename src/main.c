#include <stdio.h>
#include <SDL2/SDL.h>
#include "global.h"

int main(int argc, char **argv) {
  LED_Window *window = malloc(sizeof(LED_Window));
  if (window == NULL) {
    printf("Failed to alloc window space.\n");
    return 1;
  }
  // Until config reading is implementing, values are hardcoded.
  window->windowTitle = DEF_WIN_TITLE;
  window->windowWidth = DEF_WIN_WIDTH;
  window->windowHeight = DEF_WIN_HEIGHT;
  
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window *surface = SDL_CreateWindow(window->windowTitle, 0, 0, window->windowWidth, window->windowHeight, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(surface, -1, SDL_RENDERER_ACCELERATED);

  while (!window->shouldWindowClose) {
    SDL_Event event = {0};
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
	window->shouldWindowClose = 1;
	break;
      }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
  
  SDL_Quit();
  free(window);
  return 0;
}
