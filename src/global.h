#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Global Defines
#define DEF_WIN_WIDTH        854
#define DEF_WIN_HEIGHT       480
#define DEF_WIN_TITLE        "LED"
#define CFG_DELIMITER_SET    "::"
#define CFG_DELIMITER_POINT1 "-"
#define CFG_DELIMITER_POINT2 ">"
#define CFG_DELIMITER_SUB    '.'
#define CFG_COMMENT_CHAR     '/'
#define INIT_ALLOC_CAPACITY  256

// Global Structs
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
} LED_Window;

typedef struct InputBuffer {
    char    *text;              // Null-terminated buffer
    size_t  length;             // Current byte size of buffer
    size_t  capacity;           // Alloc'd size of buffer
    size_t  cursor;             // Cursor index in buffer
} InputBuffer;

typedef enum log_lvl {
    fatal = 0,
    error = 1,
    warn  = 2,
    debug = 3
} log_lvl;
