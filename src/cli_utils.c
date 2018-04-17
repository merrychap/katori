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


void print_invalid_option(strbuf_t *strbuf) {
    add_to_strbuf(strbuf, COLOR_RED "[-]" COLOR_RESET " Invalid option.\n");
}


void prompt_menu(int (*mode)(void), char **out_buffer) {

}


void print_strbuf(strbuf_t *strbuf) {
    for (size_t i = 0; i < strbuf->size; i++) {
        fprintf(stdout, strbuf->buf[i]);
        strbuf->buf[i] = 0;
    }
}


int add_to_strbuf(strbuf_t *strbuf, char *str) {
    if (strbuf->count >= strbuf->size) return large_index_error;
    strbuf->buf[strbuf->count++] = str;
    return 0;
}


strbuf_t * create_strbuf(size_t size) {
    strbuf_t *strbuf = (strbuf_t *) malloc(sizeof(strbuf_t));
    char     **buf   = (char **) malloc(sizeof(char *) * size);

    strbuf->buf   = buf;
    strbuf->size  = size;
    strbuf->count = 0;

    return strbuf;
}


// buffer's strings have to be freed outside of this function
int destroy_strbuf(strbuf_t *strbuf) {
    if (strbuf == NULL) return null_strbuf_error;
    
    strbuf->size = 0;
    free(strbuf->buf);
    free(strbuf);
    return 0;
}