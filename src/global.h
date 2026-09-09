#pragma once

// Global Defines
#define DEF_WIN_WIDTH        854
#define DEF_WIN_HEIGHT       480
#define DEF_WIN_TITLE        "LED"

// Global Structs
typedef struct LED_Window {
  // Bitfields instead of bools cause why not
  unsigned int shouldWindowClose : 1;
  unsigned int isModal : 1;
  unsigned int isMultiModal : 1;
  char         *windowTitle;
  int          windowWidth;
  int          windowHeight;
  char         *fontPath;
} LED_Window;
