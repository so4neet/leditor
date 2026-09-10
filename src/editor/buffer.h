#pragma once
#include "../global.h"

InputBuffer *buffer_create(void);
void line_insert_single(Line *line, size_t col, char chr);
void line_remove_single(Line *line, size_t col);
void buffer_insert_line(InputBuffer *buffer, char chr);
void buffer_split_line(InputBuffer *buffer);
void buffer_remove_line(InputBuffer *buffer);
void buffer_move_cursor(InputBuffer *buffer, int row, int column);
int buffer_load_file(InputBuffer *buffer, const char *filepath);
int buffer_save_file(InputBuffer *buffer, const char *filepath);
void buffer_clear(InputBuffer *buffer);
void destroy_buffer(InputBuffer *buffer);
void buffer_clamp_scroll(InputBuffer *buffer, GlyphAtlas *atlas, int win_width, int win_height);
void new_buffer(InputBuffer *buffer);
