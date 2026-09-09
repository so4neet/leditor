#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include "../global.h"

InputBuffer *buffer_create(void) {
    InputBuffer *buffer = calloc(1, sizeof(InputBuffer));
    buffer->capacity = INIT_ALLOC_CAPACITY;
    buffer->text     = calloc(buffer->capacity, sizeof(char));
    buffer->length   = 0;
    buffer->cursor   = 0;
    return buffer;
}

void free_buffer(InputBuffer *buffer) {
    if (!buffer) return;
    free(buffer->text);
    free(buffer);
}

void buffer_insert(InputBuffer *buffer, char chr) {
    if (buffer->length + 1 >= buffer->capacity) {
        buffer->capacity *= 2;
        buffer->text = realloc(buffer->text, buffer->capacity);
    }

    memmove(&buffer->text[buffer->cursor + 1], &buffer->text[buffer->cursor], buffer->length - buffer->cursor + 1);
    buffer->text[buffer->cursor] = chr;
    buffer->cursor++;
    buffer->length++;
}

void buffer_remove(InputBuffer *buffer) {
    if (buffer->cursor == 0) return;

    memmove(&buffer->text[buffer->cursor - 1], &buffer->text[buffer->cursor], buffer->length - buffer->cursor + 1);
    buffer->cursor--;
    buffer->length--;
}
