#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

// Global Defines
#define DEF_WIN_WIDTH        854
#define DEF_WIN_HEIGHT       480
#define DEF_WIN_TITLE        "LED"
#define CFG_DELIMITER_SET    "::"
#define CFG_DELIMITER_POINT1 "-"
#define CFG_DELIMITER_POINT2 ">"
#define CFG_DELIMITER_SUB    '.'
#define ASCII_FIRST_CHAR      32
#define ASCII_LAST_CHAR       126
#define ASCII_NUM_CHARS       (ASCII_LAST_CHAR - ASCII_FIRST_CHAR + 1)

// Global Structs

typedef struct GlyphAtlas {
    SDL_Texture *texture;
    int char_w;
    int char_h;
    int cols;
} GlyphAtlas;

typedef struct LED_Window {
  // Bitfields instead of bools cause why not
  unsigned int shouldClose : 1;
  unsigned int isModal : 1;
  unsigned int isMultiModal : 1;
  char         *title;
  int          width;
  int          height;
  char         *fontFamily;
  int          fontSize;
  SDL_Renderer *renderer;
  SDL_Window   *surface;
  TTF_Font     *font;
  GlyphAtlas   *atlas;
} LED_Window;

typedef struct Line {
    char    *data;
    size_t  length;
    size_t  capacity;
} Line;

typedef struct InputBuffer {
    Line    *lines;
    size_t  line_count;
    size_t  line_capacity;

    size_t  cursor_row;
    size_t  cursor_col;
    size_t  preferred_col;
} InputBuffer;

typedef enum log_lvl {
    fatal = 0,
    error = 1,
    warn  = 2,
    debug = 3
} log_lvl;
