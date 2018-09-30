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

typedef enum {
    SET_STR_ERROR = -1,
} cli_utils_error_t;

/////////////// COLORED STRING PROCESSING ///////////////
void red    (const char *str);
void green  (const char *str);
void yellow (const char *str);
void blue   (const char *str);
void magenta(const char *str);
void cyan   (const char *str);

void bold     (const char *str);
void underline(const char *str);

void bad (const char *str);
void good(const char *str);
void info(const char *str);
void run (const char *str);
void que (const char *str);
/////////////////////////////////////////////////////////

void term_reset(void);
void term_nonblocking(void);

void input_string(char *str, int size);
int  input_choice(void);

void clear_window(void);

FILE * reset_file(char *filename);

#endif