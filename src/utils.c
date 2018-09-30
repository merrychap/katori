#include <stdlib.h>
#include <string.h>

#include "utils.h"

void *
xmalloc(const size_t size)
{
	void * result = malloc(size);
	if (result == NULL)
		fatal("malloc error");

	return result;
}

void *
xmalloc_0(const size_t size)
{
    void * result = xmalloc(size);
	memset(result, 0, size);
	return result;
}

int
read_file(const char *filename, char **content, size_t *content_size)
{
    if (filename == NULL)     return NULL_FILENAME_PTR;
    if (*content == NULL)     return NULL_CONTENT_PTR;
    if (content_size == NULL) return NULL_CONTENT_SIZE;
    
    char ch        = '\x00';
    char *rcontent = *content;
    size_t index   = 0;
    FILE *fp       = fopen(filename, "r");

    if (fp == NULL)
        return OPEN_FILE_ERROR;

    while ((ch = fgetc(fp)) != EOF) {
        if (index < *content_size) rcontent[index++] = ch;
        else {
            char *buf = realloc(rcontent, (*content_size * 2) * sizeof(char));
            if (buf == NULL) {
                free(rcontent);
                return REALLOC_FAILED;
            }
            rcontent = buf;
            *content_size = (*content_size) * 2;
            rcontent[index++] = ch;
        }
    }
    rcontent[index] = '\x00';
    *content = rcontent;
    
    fclose(fp);
    return index - 1;
}