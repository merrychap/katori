#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "cli_utils.h"
#include "utils.h"

#define BUF_SIZE 65536

struct termios stdin_orig;

void
term_reset(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &stdin_orig);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &stdin_orig);
}

void
term_nonblocking(void)
{
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

void
input_string(char *str, int size)
{
    fgets(str, size, stdin);
    str[strlen(str) - 1] = 0;
}

int
input_choice(void)
{
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

void
clear_window(void)
{
    system("clear");
}

///////////////////////////////////////////////////////

// TODO FIX PROBLEMS WITH INCORRECT PATH OR ANYTHING LIKE THAT
FILE *
reset_file(char *filename)
{
    if (access(filename, F_OK) != -1)
        if (remove(filename) != 0) return NULL;
    // FILE ** fp = (FILE *) malloc(sizeof(FILE *));
    return fopen(filename, "a+");
}

/////////////// COLORED STRING PROCESSING ///////////////

static int
strsetopt(char *dst, const char *src,
    const char *append, const char *reset)
{
    if ((!dst) || (!src) || (!append) || (!reset))
        return SET_STR_ERROR;

    strcpy(dst, append);
    strcat(dst, src);
    strcat(dst, reset);

    return 0;
}

static int
print_ansi_str(const char *str, const char *option)
{
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    strsetopt(buf, str, option, COLOR_RESET);
    
    fprintf(stderr, "%s", buf);
    
    return 0;
}

void red    (const char *str) { print_ansi_str(str, COLOR_RED); }
void green  (const char *str) { print_ansi_str(str, COLOR_GREEN); }
void yellow (const char *str) { print_ansi_str(str, COLOR_YELLOW); }
void blue   (const char *str) { print_ansi_str(str, COLOR_BLUE); }
void magenta(const char *str) { print_ansi_str(str, COLOR_MAGENTA); }
void cyan   (const char *str) { print_ansi_str(str, COLOR_CYAN); }

void bold     (const char *str) { print_ansi_str(str, STYLE_BOLD); }
void underline(const char *str) { print_ansi_str(str, STYLE_UNDERLINE); }

void
bad(const char *str)
{
    red("[-] ");
    fprintf(stderr, "%s\n", str);
    
}

void
good(const char *str)
{
    green("[+] ");
    fprintf(stderr, "%s\n", str);
}

void
info(const char *str)
{
    yellow("[!] ");
    fprintf(stderr, "%s\n", str);
}

void
run(const char *str)
{
    bold("[~] ");
    fprintf(stderr, "%s\n", str);
}

void
que(const char *str)
{
    blue("[?] ");
    fprintf(stderr, "%s\n", str);
}

/////////////////////////////////////////////////////////