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

    input_string(buf, sizeof(buf));
    int sscanf_res = sscanf(buf, "%d", &choice);

    if ((sscanf_res == 0) || (sscanf_res == EOF)) {
        choice = -1;
    }
    return choice;
}


void print_invalid_option(void) {
    fprintf(stderr, COLOR_RED "[-]" COLOR_RESET " Invalid option.\n");
}