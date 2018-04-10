#include <stdio.h>
#include <string.h>

#include "cli_utils.h"


void input_string(char *str, int size) {
    fgets(str, size, stdin);
    str[strlen(str) - 1] = 0;
}


int input_choice() {
    int choice;
    char buf[100];

    input_string(buf, sizeof(buf));
    int sscanf_res = sscanf(buf, "%d", &choice);

    if ((sscanf_res == 0) || (sscanf_res == EOF)) {
        choice = -1;
    }
    return choice;
}


void print_invalid_option() {
    fprintf(stderr, COLOR_RED "[-]" COLOR_RESET " Invalid option.\n");
}