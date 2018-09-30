#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "spoofing.h"
#include "sniffer.h"

static void
print_welcome_header(void)
{
    clear_window();

    fprintf(stdout, COLOR_YELLOW "/////////////////////////\n" COLOR_RESET);
    fprintf(stdout, COLOR_BLUE "   katori " COLOR_RESET "greets you!\n");
    fprintf(stdout, COLOR_YELLOW "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n" COLOR_RESET);
}


static void
print_main_menu(void)
{
    fprintf(stdout, "\n=================================\n");
    fprintf(stdout, "    [1] Sniffing mode.\n");
    fprintf(stdout, "    [2] Spoofing mode (isn't implemented yet).\n");
    fprintf(stdout, "    [3] Exit.\n");
    fprintf(stdout, "=================================\n");
}


int
start_prompt(const struct katori_t *katori)
{
    int choice     = 0;
    int exit_flag  = 0;
    int first_run  = 1;

    print_welcome_header();    

    while (!exit_flag) {
        if (!first_run)
            clear_window();
        if (first_run) first_run = 0;
        
        print_main_menu();
        
        choice = input_choice();
        fprintf(stderr, "\n");

        switch (choice) {
            case 1:
                sniffing_mode(katori);
                break;
            case 2:
                spoofing_mode();
                break;
            case 3:
                exit_flag = 1;
                break;
            default:
                bad("Invalid option.");
                break;
        }
    }

    info("Shutting down...");

    return 0;
}
