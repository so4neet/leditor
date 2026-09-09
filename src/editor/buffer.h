#pragma once
#include "../global.h"

InputBuffer *buffer_create(void);
void line_insert_single(Line *line, size_t col, char chr);
void line_remove_single(Line *line, size_t col);
void line_remove_single(Line *line, size_t col);
void buffer_insert_line(InputBuffer *buffer, char chr);
void buffer_split_line(InputBuffer *buffer);
void buffer_remove_line(InputBuffer *buffer);
void buffer_move_cursor(InputBuffer *buffer, int row, int column);
