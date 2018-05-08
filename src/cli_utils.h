#ifndef _COLORS_H_
#define _COLORS_H_


#define STRBUFSIZE         50
#define STRING_APPEND_SIZE 50

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define STYLE_UNDERLINE    "\033[4m"
#define STYLE_NO_UNDERLINE "\033[24m"

#define STYLE_BOLD         "\033[1m"
#define STYLE_NO_BOLD      "\033[22m"


typedef struct strbuf_t        strbuf_t;
typedef struct strbuf_entity_t strbuf_entity_t;


typedef enum {
    large_index_error          = -1,
    null_strbuf_error          = -2,
    unable_to_remove_file      = -3,
    file_doesnt_exist          = -4,
    unable_to_open_file        = -5,
    malloc_strbuf_entity_error = -6
} cli_utils_error_t;


struct strbuf_t {
    strbuf_entity_t *buf;
    size_t size;
    size_t count;
};


struct strbuf_entity_t {
    char *entry;
    int dynalloc;
};

/////////////// COLORED STRING PROCESSING ///////////////
char * red    (char *str);
char * green  (char *str);
char * yellow (char *str);
char * blue   (char *str);
char * magenta(char *str);
char * cyan   (char *str);

char * bold     (char *str);
char * underline(char *str);

void bad (strbuf_t *strbuf, char *str);
void good(strbuf_t *strbuf, char *str);
void info(strbuf_t *strbuf, char *str);
void run (strbuf_t *strbuf, char *str);
void que (strbuf_t *strbuf, char *str);
/////////////////////////////////////////////////////////

void term_reset(void);
void term_nonblocking(void);

void input_string(char *str, int size);
int  input_choice(void);

void clear_window(void);

/////////////// STRING BUFFER FUNCTIONS ///////////////
void       print_strbuf        (strbuf_t *strbuf);
int        add_to_strbuf_str   (strbuf_t *strbuf, char const * const str);
int        add_to_strbuf_dystr (strbuf_t *strbuf, char const * const str);
int        add_to_strbuf_entity(strbuf_t *strbuf, strbuf_entity_t entity);
strbuf_t * create_strbuf       (size_t size);
int        destroy_strbuf      (strbuf_t *strbuf);
///////////////////////////////////////////////////////

FILE * reset_file(char *filename);

#endif