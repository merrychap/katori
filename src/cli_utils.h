#ifndef _COLORS_H_
#define _COLORS_H_


#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define STRBUFSIZE 50


typedef struct strbuf_t strbuf_t;


typedef enum {
    large_index_error = -1,
    null_strbuf_error = -2
} cli_utils_error_t;


struct strbuf_t {
    char **buf;
    size_t size;
    size_t count;
};



void term_reset(void);
void term_nonblocking(void);

void input_string(char *str, int size);
int  input_choice(void);

void print_invalid_option(strbuf_t *strbuf);

void print_strbuf (strbuf_t *strbuf);
int  add_to_strbuf(strbuf_t *strbuf, char *str);
strbuf_t * create_strbuf(size_t size);
int destroy_strbuf(strbuf_t *strbuf);

#endif