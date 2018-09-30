#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

typedef enum {
    OPEN_FILE_ERROR     = -1,
    REALLOC_FAILED      = -2,
    FAILED_PARSE_JSON   = -3,
    TOP_ELEM_NOT_OBJECT = -4,
    NULL_CONFIG_PTR     = -5,
    NULL_FILENAME_PTR   = -6,
    NULL_CONTENT_SIZE   = -7,
    NULL_CONTENT_PTR    = -8

} utils_error_t;

void * xmalloc(const size_t size);
void * xmalloc_0(const size_t size);

int read_file(const char *filename, char **content,
    size_t *content_size);

#endif