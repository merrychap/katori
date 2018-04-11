#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "spoofing.h"
#include "sniffing.h"


static void print_welcome_header() {
    fprintf(stdout, COLOR_YELLOW "/////////////////////////\n" COLOR_RESET);
    fprintf(stdout, COLOR_BLUE "   katori " COLOR_RESET "greets you!\n");
    fprintf(stdout, COLOR_YELLOW "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n" COLOR_RESET);
}


static void print_main_menu() {
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "    [1] Sniffing mode.\n");
    fprintf(stdout, "    [2] Spoofing mode (isn't implemented yet).\n");
    fprintf(stdout, "    [3] Exit.\n");
    fprintf(stdout, "=================================\n");
    fprintf(stdout, "Choice: ");
}


int start_prompt() {
    int err_code   = 0;
    int choice     = 0;
    int exit_flag  = 0;

    print_welcome_header();    

    while (!exit_flag) {
        print_main_menu();
        
        choice = input_choice();
        printf("\n");

        switch (choice) {
            case 1:
                sniffing_mode();
                break;
            case 2:
                spoofing_mode();
                break;
            case 3:
                exit_flag = 1;
                break;
            default:
                print_invalid_option();
                break;
        }
    }
    return err_code;
}
