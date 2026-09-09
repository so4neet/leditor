#pragma once
#include "../global.h"

InputBuffer *buffer_create(void);
void free_buffer(InputBuffer *buffer);
void buffer_insert(InputBuffer *buffer, char chr);
void buffer_remove(InputBuffer *buffer);
