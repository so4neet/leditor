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
    size_t  scroll_row;
    size_t  scroll_col;
} InputBuffer;

typedef enum log_lvl {
    fatal = 0,
    error = 1,
    warn  = 2,
    debug = 3
} log_lvl;

typedef enum EditorAction {
    ACTION_NONE = 0,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
    ACTION_NEW_BUFFER,
    ACTION_SAVE_BUFFER,
    ACTION_LOAD_BUFFER,
    ACTION_PAGE_UP,
    ACTION_PAGE_DOWN,
    ACTION_DELETE_LINE,
    ACTION_DELETE_WORD_PREV,
    ACTION_DELETE_WORD_NEXT,
    ACTION_TRIGGER_SAVE,
    ACTION_TRIGGER_LOAD,
    ACTION_CLOSE_WIN,
} EditorAction;

typedef struct KeyCombo {
    SDL_Keycode key;
    uint16_t mod;
} KeyCombo;

typedef struct ChordBinding {
    KeyCombo leader;
    KeyCombo trigger;
    EditorAction action;
} ChordBinding;

static const ChordBinding BIND_TABLE[] = {
    // Single Keypress Binds
    { {0, 0},       {SDLK_LEFT, 0},         ACTION_MOVE_LEFT },
    { {0, 0},       {SDLK_RIGHT, 0},        ACTION_MOVE_RIGHT},
    { {0, 0},       {SDLK_UP, 0},           ACTION_MOVE_UP   },
    { {0, 0},       {SDLK_DOWN, 0},         ACTION_MOVE_DOWN },

    { {0, 0},       {SDLK_h, KMOD_CTRL},    ACTION_MOVE_LEFT },
    { {0, 0},       {SDLK_l, KMOD_CTRL},    ACTION_MOVE_RIGHT},
    { {0, 0},       {SDLK_j, KMOD_CTRL},    ACTION_MOVE_DOWN },
    { {0, 0},       {SDLK_k, KMOD_CTRL},    ACTION_MOVE_UP   },

    { {SDLK_x, KMOD_CTRL}, {SDLK_s, KMOD_CTRL}, ACTION_TRIGGER_SAVE},
    { {SDLK_x, KMOD_CTRL}, {SDLK_f, KMOD_CTRL}, ACTION_TRIGGER_LOAD},
    { {SDLK_x, KMOD_CTRL}, {SDLK_c, KMOD_CTRL}, ACTION_CLOSE_WIN},
    { {SDLK_x, KMOD_CTRL}, {SDLK_n, KMOD_CTRL}, ACTION_NEW_BUFFER},
};

#define BINDING_COUNT (sizeof(BIND_TABLE) / sizeof(BIND_TABLE[0]))

typedef enum PromptType {
    PROMPT_NONE = 0,
    PROMPT_SAVE_BUFFER,
    PROMPT_LOAD_BUFFER
} PromptType;

typedef struct InputDispatcher {
    KeyCombo pending_leader;
    Uint32 leader_timestamp;

    PromptType active_prompt;
    char       prompt_buffer[256];
    size_t     prompt_len;
} InputDispatcher;
