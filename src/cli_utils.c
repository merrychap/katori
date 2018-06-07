#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "cli_utils.h"


struct termios stdin_orig;


void term_reset(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &stdin_orig);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_orig);
}


void term_nonblocking(void) { 
    struct termios term;
    
    tcgetattr(STDIN_FILENO, &stdin_orig);
    
    term = stdin_orig;
    term.c_iflag |= IGNBRK;
    
    term.c_lflag &= ~(ICANON | ECHO);// | ECHO | ECHOK | ECHOE | ECHONL | ISIG | IEXTEN);*/
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    atexit(term_reset);
}


void input_string(char *str, int size) {
    fgets(str, size, stdin);
    str[strlen(str) - 1] = 0;
}


int input_choice(void) {
    int choice;
    char buf[100];

    fprintf(stdout, "\nChoice: ");

    input_string(buf, sizeof(buf));
    int sscanf_res = sscanf(buf, "%d", &choice);

    if ((sscanf_res == 0) || (sscanf_res == EOF)) {
        choice = -1;
    }
    return choice;
}


void clear_window(void) {
    system("clear");
}


/////////////// STRING BUFFER FUNCTIONS ///////////////

static int remove_strbuf_entity(strbuf_entity_t entity) {
    if (entity.dynalloc)
        if (entity.entry) free(entity.entry);
    return 0;
}


void print_strbuf(strbuf_t *strbuf) {
    for (size_t i = 0; i < strbuf->count; i++) {
        fprintf(stdout, strbuf->buf[i].entry);
        remove_strbuf_entity(strbuf->buf[i]);
        
        strbuf->buf[i].dynalloc = 0;
        strbuf->buf[i].entry    = 0;
    }
    strbuf->count = 0;
}


int add_to_strbuf_str(strbuf_t *strbuf, char * const str) {
    if (strbuf->count >= strbuf->size) return large_index_error;
    
    strbuf->buf[strbuf->count].entry    = str;
    strbuf->buf[strbuf->count].dynalloc = 0;
    strbuf->count++;
    
    return 0;
}


int add_to_strbuf_dystr(strbuf_t *strbuf, char * const str) {
    if (strbuf->count >= strbuf->size) return large_index_error;
    
    strbuf->buf[strbuf->count].entry    = str;
    strbuf->buf[strbuf->count].dynalloc = 1;
    strbuf->count++;
    
    return 0;
}


int add_to_strbuf_entity(strbuf_t *strbuf, strbuf_entity_t entity) {
    if (strbuf->count >= strbuf->size) return large_index_error;
    strbuf->buf[strbuf->count++] = entity;
    return 0;
}


strbuf_t * create_strbuf(size_t size) {
    strbuf_t        *strbuf = (strbuf_t *)        malloc(sizeof(strbuf_t));
    strbuf_entity_t *buf    = (strbuf_entity_t *) malloc(sizeof(strbuf_entity_t) * size);

    if (strbuf == NULL || buf == NULL) return NULL;

    strbuf->buf   = buf;
    strbuf->size  = size;
    strbuf->count = 0;

    return strbuf;
}

int destroy_strbuf(strbuf_t *strbuf) {
    if (strbuf == NULL || strbuf->buf == NULL) return null_strbuf_error;

    for (size_t i = 0; i < strbuf->size; i++)
        remove_strbuf_entity(strbuf->buf[i]);

    strbuf->size = 0;
    free(strbuf->buf);
    free(strbuf);
    
    return 0;
}

///////////////////////////////////////////////////////

// TODO FIX PROBLEMS WITH INCORRECT PATH OR ANYTHING LIKE THAT
FILE * reset_file(char *filename) {
    if (access(filename, F_OK) != -1)
        if (remove(filename) != 0) return NULL;
    // FILE ** fp = (FILE *) malloc(sizeof(FILE *));
    return fopen(filename, "a+");
}


/////////////// COLORED STRING PROCESSING ///////////////

// BAD IMPLEMENTATION??

// USING DYNAMIC ALLOCATION EVERYWHERE IS A BAD PRACTISE
// CONSIDER SOME COMMON BUFFER FOR ALL (MAYBE USING STRBUF HERE)???
// ALSO MAYBE PRINT FUNCTION SHOULD BE ADDED INSTEAD OF ADD TO STRINGBUF???

static char * set_str(char * str, char *append, char *reset) {
    size_t size = strlen(str) + STRING_APPEND_SIZE;
    char * pstr = (char *) malloc(size);
    memset(pstr, 0, size);

    strcpy(pstr, append);
    strcat(pstr, str);
    strcat(pstr, reset);

    return pstr;
}

char * red    (char *str) { return set_str(str, COLOR_RED,     COLOR_RESET); }
char * green  (char *str) { return set_str(str, COLOR_GREEN,   COLOR_RESET); }
char * yellow (char *str) { return set_str(str, COLOR_YELLOW,  COLOR_RESET); }
char * blue   (char *str) { return set_str(str, COLOR_BLUE,    COLOR_RESET); }
char * magenta(char *str) { return set_str(str, COLOR_MAGENTA, COLOR_RESET); }
char * cyan   (char *str) { return set_str(str, COLOR_CYAN,    COLOR_RESET); }

char * bold     (char *str) { return set_str(str, STYLE_BOLD,      STYLE_NO_BOLD); }
char * underline(char *str) { return set_str(str, STYLE_UNDERLINE, STYLE_NO_UNDERLINE); }

void bad(strbuf_t *strbuf, char *str) {
    add_to_strbuf_dystr(strbuf, bold(red("[-] ")));
    add_to_strbuf_str  (strbuf, str);
}

void good(strbuf_t *strbuf, char *str) {
    add_to_strbuf_dystr(strbuf, bold(green("[+] ")));
    add_to_strbuf_str  (strbuf, str);
}

void info(strbuf_t *strbuf, char *str) {
    add_to_strbuf_dystr(strbuf, bold(yellow("[!] ")));
    add_to_strbuf_str  (strbuf, str);
}

void run(strbuf_t *strbuf, char *str) {
    add_to_strbuf_dystr(strbuf, bold("[~] "));
    add_to_strbuf_str  (strbuf, str);
}

void que(strbuf_t *strbuf, char *str) {
    add_to_strbuf_dystr(strbuf, bold(blue("[?] ")));
    add_to_strbuf_str  (strbuf, str);
}

/////////////////////////////////////////////////////////