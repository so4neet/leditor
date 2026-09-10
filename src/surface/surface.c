#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <fontconfig/fontconfig.h>
#include "../logger/logger.h"
#include "../global.h"

static char *find_font_path(const char *fname) {
    if (!fname || fname[0] == '\0') return NULL;

    FcConfig *config = FcInitLoadConfigAndFonts();  // ridiculously long function name wtf
    if (!config) return NULL;

    FcPattern *pattern = FcNameParse((const FcChar8 *)fname);
    if (!pattern) {
        FcConfigDestroy(config);
        return NULL;
    }

    FcConfigSubstitute(config, pattern, FcMatchPattern);
    FcDefaultSubstitute(pattern);

    FcResult result;
    FcPattern *match = FcFontMatch(config, pattern, &result);
    char *font_path = NULL;

    if (match) {
        FcChar8 *file = NULL;
        // Get absolute path for closest font found
        if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch) {
            size_t len = strlen((const char *)file) + 1;
            font_path = malloc(len);
            if (font_path) {
                memcpy(font_path, file, len);
            }
        }
        FcPatternDestroy(match);
    }
    FcPatternDestroy(pattern);
    FcConfigDestroy(config);
    return font_path;
}

int LED_Init_Window(LED_Window *window) {
    // Until config reading is implementing, values are hardcoded.
    window->title = DEF_WIN_TITLE;
    window->width = DEF_WIN_WIDTH;
    window->height = DEF_WIN_HEIGHT;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      l_fatal("SDL_INIT_VIDEO Failure: %s", SDL_GetError());
      return 1;
    }
    if (TTF_Init() != 0) {
        l_fatal("TTF_Init Failure: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    window->surface = SDL_CreateWindow(window->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->width, window->height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    window->renderer = SDL_CreateRenderer(window->surface, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);
    char *fpath = find_font_path(window->fontFamily);
    if (fpath) {
        l_debug("Resolved font '%s' to '%s'", window->fontFamily, fpath);

        window->font = TTF_OpenFont(fpath, window->fontSize > 0 ? window->fontSize : 12);
        if (!window->font) {
            l_error("Failed to load font '%s'", TTF_GetError());
        }
        free(fpath);
    } else {
        l_warn("Couldn't find system font for '%s'. Falling back to default.", window->fontFamily);
    }
    return 0;
}
