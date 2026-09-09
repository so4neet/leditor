#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>
#include "../global.h"

InputBuffer *buffer_create(void) {
    InputBuffer *buffer = calloc(1, sizeof(InputBuffer));
    buffer->line_capacity = 8;
    buffer->lines = calloc(buffer->line_capacity, sizeof(Line));

    buffer->lines[0].capacity = 32;
    buffer->lines[0].data = calloc(buffer->lines[0].capacity, sizeof(char));
    buffer->lines[0].length = 0;
    return buffer;
}

void line_insert_single(Line *line, size_t col, char chr) {
    if (line->length + 1 >= line->capacity) {
        line->capacity = line->capacity ? line->capacity * 2 : 16;
        line->data = realloc(line->data, line->capacity);
    }
    memmove(&line->data[col + 1], &line->data[col], line->length - col + 1);
    line->data[col] = chr;
    line->length++;
}

void line_remove_single(Line *line, size_t col) {
    if (col >= line->length) return;
    memmove(&line->data[col], &line->data[col + 1], line->length - col);
    line->length--;
}

void buffer_insert_line(InputBuffer *buffer, char chr) {
    Line *cur = &buffer->lines[buffer->cursor_row];
    line_insert_single(cur, buffer->cursor_col, chr);
    buffer->cursor_col++;
    buffer->preferred_col = buffer->cursor_col;
}

void buffer_split_line(InputBuffer *buffer) {
    // Check memory and realloc if needed
    if (buffer->line_count + 1 >= buffer->line_capacity) {
        buffer->line_capacity *= 2;
        buffer->lines = realloc(buffer->lines, buffer->line_capacity * sizeof(Line));
    }

    Line *cur = &buffer->lines[buffer->cursor_row];
    memmove(&buffer->lines[buffer->cursor_row + 2],
            &buffer->lines[buffer->cursor_row + 1],
            (buffer->line_count - buffer->cursor_row - 1) * sizeof(Line));

    Line *next = &buffer->lines[buffer->cursor_row + 1];
    size_t split_len = cur->length - buffer->cursor_col;

    next->capacity = split_len + 16;
    next->data = calloc(next->capacity, sizeof(char));
    next->length = split_len;

    // Move rest of current line data to next line
    if (split_len > 0) {
        memcpy(next->data, &cur->data[buffer->cursor_col], split_len);
        cur->data[buffer->cursor_col] = '\0';
        cur->length = buffer->cursor_col;
    }

    buffer->line_count++;
    buffer->cursor_row++;
    buffer->cursor_col = 0; // Reset cursor to left of screen
    buffer->preferred_col = 0;
}

void buffer_remove_line(InputBuffer *buffer) {
    Line *cur = &buffer->lines[buffer->cursor_row];

    if (buffer->cursor_col > 0) {
        // remove character
        line_remove_single(cur, buffer->cursor_col - 1);
        buffer->cursor_col--;
    } else if (buffer->cursor_row > 0) {
        // merge line into line above
        Line *prev = &buffer->lines[buffer->cursor_row - 1];
        size_t old_prev_len = prev->length;

        // resize line above to hold current line contents
        if (prev->length + cur->length + 1 > prev->capacity) {
            // realloc
            prev->capacity = prev->length + cur->length + 16;
            prev->data = realloc(prev->data, prev->capacity);
        }

        // copy line to end of prev
        memcpy(&prev->data[old_prev_len], cur->data, cur->length + 1);
        prev->length += cur->length;

        // free current line and remove it
        free(cur->data);
        memmove(&buffer->lines[buffer->cursor_row],
                &buffer->lines[buffer->cursor_row + 1],
                (buffer->line_count - buffer->cursor_row - 1) * sizeof(Line));
        buffer->line_count--;
        buffer->cursor_row--;
        buffer->cursor_col = old_prev_len;
    }
    buffer->preferred_col = buffer->cursor_col;
}
